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

namespace mcld
{

/** \class LinearAllocator
 *  \brief LinearAllocator is a allocator that follows two-phase memory
 *  allocation.
 *
 *  Two-phase memory allocation clear separates the allocation of memory into
 *  'allocation' and 'deallocation' phases. There are no interleaving
 *  allocation and deallocation in the use.
 *
 *  template argument DataType is the DataType to be allocated
 *  template argument DataNum is the number of data to be pre-allocated while
 *  construction.
 */
template<typename DataType, size_t DataNum>
class LinearAllocator
{
public:
  typedef std::allocator<DataType>            DataAlloc;
  typedef typename DataAlloc::pointer         pointer;
  typedef typename DataAlloc::const_pointer   const_pointer;
  typedef typename DataAlloc::reference       reference;
  typedef typename DataAlloc::const_reference const_reference;
  typedef typename DataAlloc::type_value      type_value;
  typedef typename DataAlloc::size_type       size_type;
  typedef typename DataAlloc::difference_type difference_type;
  typedef unsigned char                       byte_type;

public:
  LinearAllocator()
    : m_DataAlloc(),
      m_pRoot(0),
      m_pCurrent(0),
      m_FirstFree(0),
      m_AllocatedNum(0) {
  }

  LinearAllocator(const LinearAllocator& pCopy)
    : m_DataAlloc(pCopy.m_DataAlloc),
      m_FirstFree(pCopy.m_FirstFree),
      m_AllocateNum(0) {
    if (0 == pCopy.m_pRoot) {
      m_pRoot = 0;
      m_pCurrent = 0;
      return;
    }
    m_pRoot = createChunk();
    copyChunkData(m_pRoot, const_cast<const Chunk*>(pCopy.m_pRoot));
    // ---- //
    Chunk *copy_cur = pCopy.m_pRoot->next;
    while (0 != copy_cur) {
      createChunk();
      copyChunkData(m_pCurrent, const_cast<const Chunk*>(copy_cur));
      copy_cur = copy_cur->next;
    }
  }

  virtual ~LinearAllocator() {
  }

  pointer address(reference X) const
  { return &X; }

  const_pointer address(const_reference X) const
  { return &X; }

  /// allocate - allocate N data.
  //  disallow to allocate a chunk whose size is bigger than a chunk.
  //  @param N the number of allocated data.
  //  @return the start address of the allocated memory
  pointer allocate(size_type N) {
    if (0 == N || N > ElementNum)
      return 0;

    size_type rest_num_elem = ElementNum - m_FirstFree + 1;
    if (N > rest_num_elem) {
      createChunk();
      m_FirstFree = 0;
    }
    return &(m_pCurrent->data[m_FirstFree]);
  }

  size_type max_size() const
  { return m_AllocatedNum; }

  /// standard construct - construct data on arbitrary address
  //  @param pPtr the address where the object to be constructed
  //  @param pValue the value to be constructed
  void construct(pointer pPtr, const_reference pValue) {
    //if (!isIn(pPtr))
    //  return;
    m_DataAlloc.construct(pPtr, pValue);
  }

  /// standard destroy - destroy data on arbitrary address
  //  @para pPtr the address where the data to be destruected.
  void destroy(pointer pPtr) {
    //if (!isIn(pPtr))
    //  return;
    m_DataAlloc.destroy(pPtr);
  }

  /// construct - overloading of standard construct. construct data on
  //  the first free element
  //  @param pValue the value to be constructed on the first element.
  pointer construct(const_reference pValue) {
    pointer result = &(m_pCurrent->data[m_FirstFree]);
    m_DataAlloc.construct( result, pValue);
    if (m_FirstFree == ElementNum) {
      createChunk();
      m_FirstFree = 1;
    }
    else
      ++m_FirstFree;
    return result;
  }

  /// destroy - overloading of standard destroy. destroy data on
  //  the first free element
  void destroy() {
    m_DataAlloc.destroy(&(m_pCurrent->data[m_FirstFree]));
    if (m_FirstFree > 0)
      --m_FirstFree;
  }

  bool empty() const {
    return (0 == m_pRoot);
  }
protected:
  enum { MaxBytes = (size_type)sizeof(DataType)*(size_type)DataNum };
  enum { ElementBytes = (size_type)sizeof(DataType) };
  enum { ElementNum = (size_type)DataNum };
  enum { FirstElement = 0 };
  enum { LastElement = ElementNum - 1 };

protected:
  struct Chunk {
    byte_type volatile data[MaxBytes];
    Chunk *next;
  };

protected:
  Chunk *createChunk() {
    Chunk* result = (Chunk*)malloc(sizeof(Chunk));
    result->next = 0;
    m_pCurrent->next = result;
    m_pCurrent = result;
    m_AllocatedNum += ElementNum;
    return result;
  }

  void copyChunkData(Chunk* pTo, const Chunk* pFrom) const {
    for (size_t i=0; i<ElementNum; ++i) {
      pTo->data[i] = pFrom->data[i];
    }
  }

  bool isIn(pointer pPtr) {
    Chunk* cur = m_pRoot;
    while (0 != cur) {
      if (pPtr >= &(cur->data[FirstElement]) && 
          pPtr <= &(cur->data[LastElement]))
        return true;
      cur = cur->next;
    }
    return false;
  }
protected:
  DataAlloc m_DataAlloc;
  Chunk *m_pRoot;
  Chunk *m_pCurrent;
  size_type m_FirstFree;
  size_type m_AllocatedNum;
};

} // namespace of mcld

#endif

