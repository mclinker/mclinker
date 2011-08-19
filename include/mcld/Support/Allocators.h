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
 *  \brief Chunk is the basic unit of the storage of the LinearAllocator
 *
 *  @see LinearAllocator
 */
template<typename DataType, size_t ChunkSize>
struct Chunk
{
public:
  typedef DataType value_type;
public:
  Chunk()
  : next(0), bound(0)
  { }

  static size_t size() { return ChunkSize; }

public:
  Chunk* next;
  size_t bound;
  DataType data[ChunkSize];
};

template<typename DataType>
struct Chunk<DataType, 0>
{
public:
  typedef DataType value_type;

public:
  Chunk()
  : next(0), bound(0) {
    if (0 != m_Size)
      data = (DataType*)malloc(sizeof(DataType)*m_Size);
    else
      data = 0;
  }

  ~Chunk() {
    if (data)
      free(data);
  }

  static size_t size()              { return m_Size; }
  static void setSize(size_t pSize) { m_Size = pSize; }

public:
  Chunk* next;
  size_t bound;
  DataType *data;
  static size_t m_Size;
};

template<typename DataType>
size_t Chunk<DataType, 0>::m_Size = 0;

template<typename ChunkType>
class LinearAllocatorBase : private Uncopyable
{
public:
  typedef ChunkType                             chunk_type;
  typedef typename ChunkType::value_type        value_type;
  typedef typename ChunkType::value_type*       pointer;
  typedef typename ChunkType::value_type&       reference;
  typedef const typename ChunkType::value_type* const_pointer;
  typedef const typename ChunkType::value_type& const_reference;
  typedef size_t                                size_type;
  typedef ptrdiff_t                             difference_type;
  typedef unsigned char                         byte_type;

protected:
  LinearAllocatorBase()
    : m_pRoot(0),
      m_pCurrent(0),
      m_AllocatedNum(0) {
  }

  virtual ~LinearAllocatorBase()
  { }

public:
  pointer address(reference X) const
  { return &X; }

  const_pointer address(const_reference X) const
  { return &X; }

  /// standard construct - constructing an object on the location pointed by
  //  pPtr, and using its copy constructor to initialized its value to pValue.
  //
  //  @param pPtr the address where the object to be constructed
  //  @param pValue the value to be constructed
  void construct(pointer pPtr, const_reference pValue)
  { new (pPtr) value_type(pValue); }

  /// default construct - constructing an object on the location pointed by
  //  pPtr, and using its default constructor to initialized its value to
  //  pValue.
  //
  //  @param pPtr the address where the object to be constructed
  void construct(pointer pPtr)
  { new (pPtr) value_type(); }

  /// standard destroy - destroy data on arbitrary address
  //  @para pPtr the address where the data to be destruected.
  void destroy(pointer pPtr)
  { pPtr->~value_type(); }

  /// allocate - allocate N data in order.
  //  - Disallow to allocate a chunk whose size is bigger than a chunk.
  //
  //  @param N the number of allocated data.
  //  @return the start address of the allocated memory
  pointer allocate(size_type N) {
    if (0 == N || N > chunk_type::size())
      return 0;

    if (empty())
      initialize();

    size_type rest_num_elem = chunk_type::size() - m_pCurrent->bound;
    pointer result = 0;
    if (N > rest_num_elem)
      createChunk();
    result = const_cast<pointer>(&(m_pCurrent->data[m_pCurrent->bound]));
    m_pCurrent->bound += N;
    return result;
  }

  /// allocate - clone function of allocating one datum.
  pointer allocate() {
    if (empty())
      initialize();

    pointer result = 0;
    if (chunk_type::size() == m_pCurrent->bound)
      createChunk();
    result = const_cast<pointer>(&(m_pCurrent->data[m_pCurrent->bound]));
    ++m_pCurrent->bound;
    return result;
  }

  /// deallocate - deallocate N data from the pPtr
  //  - if we can simply release some memory, then do it. Otherwise, do 
  //    nothing.
  void deallocate(pointer &pPtr, size_type N) {
    if (0 == N || 
        N > chunk_type::size() || 
        0 == m_pCurrent->bound || 
        N >= m_pCurrent->bound)
      return;
    if (!isAvaliable(pPtr))
      return;
    m_pCurrent->bound -= N;
    pPtr = 0;
  }

  /// deallocate - clone function of deallocating one datum
  void deallocate(pointer &pPtr) {
    if (0 == m_pCurrent->bound)
      return;
    if (!isAvaliable(pPtr))
      return;
    m_pCurrent->bound -= 1;
    pPtr = 0;
  }

  /// isIn - whether the pPtr is in the current chunk?
  bool isIn(pointer pPtr) const {
    if (pPtr >= &(m_pCurrent->data[0]) && 
        pPtr <= &(m_pCurrent->data[chunk_type::size()-1]))
      return true;
    return false;
  }

  /// isIn - whether the pPtr is allocated, and can be constructed.
  bool isAvailable(pointer pPtr) const {
    if (pPtr >= &(m_pCurrent->data[m_pCurrent->bound]) && 
        pPtr <= &(m_pCurrent->data[chunk_type::size()-1]))
      return true;
    return false;
  }

  void reset() {
    m_pRoot = 0;
    m_pCurrent = 0;
    m_AllocatedNum = 0;
  }

  /// clear - clear all chunks
  void clear() {
    chunk_type *cur = m_pRoot, *prev;
    while (0 != cur) {
      int idx=0;
      prev = cur;
      cur = cur->next;
      while (idx != prev->bound) {
        destroy(&prev->data[idx]);
        ++idx;
      }
      delete prev;
    }
    reset();
  }

  // -----  observers  ----- //
  bool empty() const {
    return (0 == m_pRoot);
  }

  size_type max_size() const
  { return m_AllocatedNum; }

protected:
  inline void initialize() {
    m_pRoot = new chunk_type();
    m_pCurrent = m_pRoot;
    m_AllocatedNum += chunk_type::size();
  }

  inline chunk_type *createChunk() {
    chunk_type *result = new chunk_type();
    m_pCurrent->next = result;
    m_pCurrent = result;
    m_AllocatedNum += chunk_type::size();
    return result;
  }

protected:
  chunk_type *m_pRoot;
  chunk_type *m_pCurrent;
  size_type   m_AllocatedNum;
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
class LinearAllocator : public LinearAllocatorBase<Chunk<DataType, ChunkSize> >
{
public:
  LinearAllocator()
    : LinearAllocatorBase<Chunk<DataType, ChunkSize> >() {
  }

  virtual ~LinearAllocator()
  { }
};

template<typename DataType>
class LinearAllocator<DataType, 0> : public LinearAllocatorBase<Chunk<DataType, 0> >
{
public:
  explicit LinearAllocator(size_t pNum)
    : LinearAllocatorBase<Chunk<DataType, 0> >() {
    Chunk<DataType, 0>::setSize(pNum);
  }

  virtual ~LinearAllocator()
  { }
};

} // namespace mcld

#endif

