/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_TREE_ALLOCATOR_H
#define MCLD_TREE_ALLOCATOR_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <set>
#include <mcld/ADT/TreeBase.h>
#include <mcld/ADT/Allocators.h>

namespace mcld
{

/** \class NodeFactory
 *  \brief NodeFactory manages the creation and destruction of mcld::Node.
 *
 *  NodeFactory guarantees all allocated memory are released finally. When
 *  the destructor of NodeFactory is called, all allocated memory are freed.
 *
 *  NodeFactory provides delegation of memory. Sometimes, we have to merge two
 *  NodeFactories, and NodeFactory::delegate() can move the memory from one
 *  NodeFactories to another.
 *  
 *  @see LinearAllocator
 */
template<typename DataType>
class NodeFactory : private LinearAllocator<typename mcld::Node<DataType>, 64>
{
  NodeFactory(const NodeFactory& pCopy); /// NOT TO IMPLEMENT
  NodeFactory& operator=(const NodeFactory& pCopy); /// NOT TO IMPLEMENT

private:
  typedef LinearAllocator<typename mcld::Node<DataType>, 64> Alloc;

public:
  typedef Node<DataType>           NodeType;

  /// iterator
  class iterator
  {
    friend class NodeFactory;
    iterator(typename Alloc::Chunk* pInChunk, unsigned int pPos)
    : m_pInChunk(pInChunk), m_Pos(pPos)
    { }

  public:
    iterator()
    : m_pInChunk(0), m_Pos(0)
    { }

    ~iterator()
    { }

    iterator& next() {
      if (Alloc::ElementNum == m_Pos) {
        m_pInChunk = m_pInChunk->next;
        m_Pos = 0;
      }
      else
        ++m_Pos;
      return *this;
    }

    DataType* get() {
      return &(m_pInChunk->data[m_Pos]);
    }

  private:
    typename Alloc::Chunk* m_pInChunk;
    unsigned int m_Pos;
  };

  /// const_iterator
  class const_iterator
  {
    friend class NodeFactory;
    const_iterator(typename const Alloc::Chunk* pInChunk, unsigned int pPos)
    : m_pInChunk(const_cast<typename Alloc::Chunk*>(pInChunk)), m_Pos(pPos)
    { }
  public:
    const_iterator()
    : m_pInChunk(0), m_Pos(0)
    { }

    ~const_iterator()
    { }

    const_iterator& next() {
      if (Alloc::ElementNum == m_Pos) {
        m_pInChunk = m_pInChunk->next;
        m_Pos = 0;
      }
      else
        ++m_Pos;
      return *this;
    }

    const DataType* get() const {
      return const_cast<const DataType*>(&(m_pInChunk->data[m_Pos]));
    }

  private:
    typename Alloc::Chunk* m_pInChunk;
    unsigned int m_Pos;
  };

public:
  NodeFactory()
  : Alloc(), m_NodeNum(0)
  { }

  /// destructor - release all controlled memory.
  virtual ~NodeFactory()
  { Alloc::clear(); }

  NodeType* produce() {
    ++m_NodeNum;
    return Alloc::allocate();
  }

  unsigned int size() const {
    return m_NodeNum;
  }

  /// delegate - get the control of chunks owned by the client
  //  after calling delegate(), client will renouce its control
  //  of memory space.
  void delegate(NodeFactory& pClient) {
    if (pClient.empty())
      return;

    if (empty()) {
      replace(pClient);
      pClient.renounce();
      return;
    }

    // neither is empty
    concatenate(pClient);
    pClient.renounce();
  }

  // -----  iterators  ----- //
  iterator begin() {
    return iterator(this, m_pRoot, 0);
  }

  const_iterator begin() const {
    return const_iterator(this, m_pRoot, 0);
  }

  iterator end() {
    return iterator(this, m_pCurrent, m_FirstFree);
  }

  const_iterator end() const {
    return const_iterator(this, m_pCurrent, m_FirstFree);
  }

private:
  /// renounce - give up the control of all chunks
  void renounce() {
    Alloc::m_pRoot = 0;
    Alloc::m_pCurrent = 0;
    Alloc::m_FirstFree = Alloc::m_AllocatedNum = m_NodeNum = 0;
  }

  /// replace - be the agent of client.
  void replace(NodeFactory& pClient) {
    Alloc::m_pRoot = pClient.Alloc::m_pRoot;
    Alloc::m_pCurrent = pClient.Alloc::m_pCurrent;
    Alloc::m_FirstFree = pClient.Alloc::m_FirstFree;
    Alloc::m_AllocatedNum = pClient.Alloc::m_AllocatedNum;
    m_NodeNum = pClient.m_NodeNum;
  }

  /// concatenate - conncet two factories
  void concatenate(NodeFactory& pClient) {
    Alloc::m_pCurrent->next = pClient.Alloc::m_pRoot;
    Alloc::m_pCurrent = pClient.Alloc::m_pCurrent;
    Alloc::m_FirstFree = pClient.Alloc::m_FirstFree;
    Alloc::m_AllocatedNum += pClient.Alloc::m_AllocatedNum;
    m_NodeNum += pClient.m_NodeNum;
  }

private:
  unsigned int m_NodeNum;
};

} // namespace of mcld

#endif

