/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef LLVM_ALLOCATORS_H
#define LLVM_ALLOCATORS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <cstdlib>
#include <mcld/ADT/Uncopyable.h>
#include <mcld/ADT/TypeTraits.h>

namespace mcld
{

/** \class Chunk
 *  \brief Chunk is the basict unit of the storage of the LinearAllocator
 *
 *  @see LinearAllocator
 */
template<typename DataType, size_t ChunkSize>
struct Chunk
{
public:
  typedef DataType value_type;
  enum { chunk_size = ChunkSize };

public:
  Chunk()
  : bound(0), next(0)
  { }

public:
  Chunk *next;
  DataType data[ChunkSize];
  size_t bound;
};

/** \class LinearAllocator
 *  \brief LinearAllocator is another bump pointer allocator which should be
 *  limited in use of two-phase memory allocation.
 *
 *  Two-phase memory allocation clear separates the use of memory into 'claim'
 *  and 'release' phases. There are no interleaving allocation and
 *  deallocation. Interleaving 'allocate' and 'deallocate' increases the size
 *  of allocated memory, and causes bad locality.
 *
 *  The underlying concept of LinearAllocator is a memory pool. LinearAllocator
 *  is a simple implementation of boost::pool's ordered_malloc() and
 *  ordered_free().
 *
 *  template argument DataType is the DataType to be allocated
 *  template argument ChunkSize is the number of bytes of a chunk
 */
template<typename DataType, size_t ChunkSize>
class LinearAllocator : private Uncopyable
{
public:
  typedef DataType*                  pointer;
  typedef DataType&                  reference;
  typedef const DataType*            const_pointer;
  typedef const DataType&            const_reference;
  typedef DataType                   value_type;
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  typedef unsigned char              byte_type;
  typedef Chunk<DataType, ChunkSize> chunk_type;

protected:
  enum { ElementNum = (size_type)ChunkSize };
  enum { FirstElement = 0 };
  enum { LastElement = ElementNum - 1 };

public:
  LinearAllocator()
    : m_pRoot(0),
      m_pCurrent(0),
      m_AllocatedNum(0) {
  }

  virtual ~LinearAllocator()
  { }

  pointer address(reference X) const
  { return &X; }

  const_pointer address(const_reference X) const
  { return &X; }

  /// allocate - allocate N data in order.
  //  - Disallow to allocate a chunk whose size is bigger than a chunk.
  //
  //  @param N the number of allocated data.
  //  @return the start address of the allocated memory
  pointer allocate(size_type N) {
    if (0 == N || N > ElementNum)
      return 0;

    if (0 == m_pRoot)
      initialize();

    size_type rest_num_elem = ElementNum - m_pCurrent->bound;
    pointer result = 0;
    if (N > rest_num_elem)
      createChunk();
    result = const_cast<pointer>(&(m_pCurrent->data[m_pCurrent->bound]));
    m_pCurrent->bound += N;
    return result;
  }

  /// allocate - clone function of allocating one datum.
  pointer allocate() {
    if (0 == m_pRoot)
      initialize();

    pointer result = 0;
    if (ElementNum == m_pCurrent->bound)
      createChunk();
    result = const_cast<pointer>(&(m_pCurrent->data[m_pCurrent->bound]));
    ++m_pCurrent->bound;
    return result;
  }

  /// deallocate - deallocate N data from the pPtr
  //  - if we can simply release some memory, then do it. Otherwise, do 
  //    nothing.
  void deallocate(pointer pPtr, size_type N) {
    if (0 == N || N > ElementNum || 0 == m_pCurrent->bound || N >= m_pCurrent->bound)
      return;
    if (!isAvaliable(pPtr))
      return;
    m_pCurrent->bound -= N;
  }

  /// deallocate - clone function of deallocating one datum
  void deallocate(pointer pPtr) {
    if (0 == m_pCurrent->bound)
      return;
    if (!isAvaliable(pPtr))
      return;
    m_pCurrent->bound -= 1;
  }

  size_type max_size() const
  { return m_AllocatedNum; }

  /// standard construct - constructing an object on the location pointed by
  //  pPtr, and using its copy constructor to initialized its value to pValue.
  //
  //  @param pPtr the address where the object to be constructed
  //  @param pValue the value to be constructed
  void construct(pointer pPtr, const_reference pValue)
  { new (pPtr) DataType(pValue); }

  /// default construct - constructing an object on the location pointed by
  //  pPtr, and using its default constructor to initialized its value to
  //  pValue.
  //
  //  @param pPtr the address where the object to be constructed
  void construct(pointer pPtr)
  { new (pPtr) DataType(); }

  /// standard destroy - destroy data on arbitrary address
  //  @para pPtr the address where the data to be destruected.
  void destroy(pointer pPtr)
  { pPtr->~DataType(); }

  bool empty() const {
    return (0 == m_pRoot);
  }

  /// clear - clear all chunks
  void clear() {
    // call all destructors
    ChunkType *cur = m_pRoot, *prev;
    while (0 != cur) {
      int idx=0;
      while (idx != cur->bound) {
        destroy(&cur->data[idx]);
        ++idx;
      }
      cur = cur->next;
    }

    // free all chunks
    while (0 != cur) {
      prev = cur;
      cur = cur->next;
      free(prev);
    }

    // reset
    m_pRoot = 0;
    m_pCurrent = 0;
    m_AllocatedNum = 0;
  }

  /// isIn - whether the pPtr is in the current chunk?
  bool isIn(pointer pPtr) const {
    if (pPtr >= &(m_pCurrent->data[FirstElement]) && 
        pPtr <= &(m_pCurrent->data[LastElement]))
      return true;
    return false;
  }

  /// isIn - whether the pPtr is allocated, and can be constructed.
  bool isAvailable(pointer pPtr) const {
    if (pPtr >= &(m_pCurrent->data[m_pCurrent->bound]) && 
        pPtr <= &(m_pCurrent->data[LastElement]))
      return true;
    return false;
  }

protected:
  typedef Chunk<DataType, ChunkSize> ChunkType;

protected:
  inline void initialize() {
    m_pRoot = (ChunkType*)malloc(sizeof(ChunkType));
    new (m_pRoot) ChunkType();
    m_pCurrent = m_pRoot;
    m_pCurrent->next = 0;
    m_AllocatedNum += ElementNum;
  }

  inline ChunkType *createChunk() {
    ChunkType* result = (ChunkType*)malloc(sizeof(ChunkType));
    new (result) ChunkType();
    m_pCurrent->next = result;
    m_pCurrent = result;
    m_pCurrent->next = 0;
    m_AllocatedNum += ElementNum;
    return result;
  }

protected:
  ChunkType *m_pRoot;
  ChunkType *m_pCurrent;
  size_type m_AllocatedNum;
};

} // namespace of mcld

#endif

