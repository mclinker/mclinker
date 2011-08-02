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

template<typename ChunkType>
struct DataIteratorBase
{
public:
  ChunkType* m_pChunk;
  unsigned int m_Pos;

public:
  DataIteratorBase(ChunkType* X, unsigned int pPos)
  : m_pChunk(X), m_Pos(pPos)
  { }

  inline void advance() {
    ++m_Pos;
    if ((m_Pos == m_pChunk->bound) && (0 == m_pChunk->next))
      return;
    if (m_Pos == m_pChunk->bound) {
      m_pChunk = m_pChunk->next;
      m_Pos = 0;
    }
  }

  bool operator==(const DataIteratorBase& y) const
  { return ((this->m_pChunk == y.m_pChunk) && (this->m_Pos == y.m_Pos)); }

  bool operator!=(const DataIteratorBase& y) const
  { return ((this->m_pChunk != y.m_pChunk) || (this->m_Pos != y.m_Pos)); }
};

/** \class DataIterator
 *  \brief DataIterator provides STL compatible iterator for allocators
 */
template<typename ChunkType, class Traits>
class DataIterator : public DataIteratorBase<ChunkType>
{
public:
  typedef typename ChunkType::value_type  value_type;
  typedef Traits                          traits;
  typedef typename traits::pointer        pointer;
  typedef typename traits::reference      reference;
  typedef DataIterator<ChunkType, Traits> Self;
  typedef DataIteratorBase<ChunkType>     Base;

  typedef typename traits::nonconst_traits         nonconst_traits;
  typedef DataIterator<ChunkType, nonconst_traits> iterator;
  typedef typename traits::const_traits            const_traits;
  typedef DataIterator<ChunkType, const_traits>    const_iterator;
  typedef std::forward_iterator_tag                iterator_category;
  typedef size_t                                   size_type;
  typedef ptrdiff_t                                difference_type;

public:
  DataIterator()
  : Base(0, 0)
  { }

  DataIterator(ChunkType* pChunk, unsigned int pPos)
  : Base(pChunk, pPos)
  { }

  DataIterator(const DataIterator& pCopy)
  : Base(pCopy.m_pChunk, pCopy.m_Pos)
  { }

  ~DataIterator()
  { }

  // -----  operators  ----- //
  reference operator*() {
    if (0 == Base::m_pChunk)
      assert(0 && "data iterator goes to a invalid position");
    return Base::m_pChunk->data[Base::m_Pos];
  }

  Self& operator++() {
    this->Base::advance();
    return *this;
  }

  Self operator++(int) {
    Self tmp = *this;
    this->Base::advance();
    return tmp;
  }
};

/** \class LinearAllocator
 *  \brief LinearAllocator is a simple allocator which is limited in use of
 *  two-phase memory allocation.
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
  typedef DataIterator<chunk_type, NonConstTraits<value_type> > iterator;
  typedef DataIterator<chunk_type, ConstTraits<value_type> >    const_iterator;

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
    iterator data, dEnd = end();
    for (data = begin(); data!=dEnd; ++data) {
      destroy(&(*data));
    }

    // free all chunks
    ChunkType *cur = m_pRoot, *prev;
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

  // -----  iterators  ----- //
  iterator begin()
  { return iterator(m_pRoot, 0); }

  const_iterator begin() const
  { return const_iterator(m_pRoot, 0); }

  iterator end()
  { return iterator(m_pCurrent, m_pCurrent->bound); }

  const_iterator end() const
  { return const_iterator(m_pCurrent, m_pCurrent->bound); }

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

