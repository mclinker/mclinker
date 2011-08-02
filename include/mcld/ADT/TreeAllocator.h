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
#include <mcld/ADT/Allocators.h>
#include <mcld/ADT/TreeBase.h>

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
class NodeFactory : private LinearAllocator<Node<DataType>, 64>
{
private:
  typedef LinearAllocator<Node<DataType>, 64> Alloc;

public:
  typedef Node<DataType>                 NodeType;
  typedef typename Alloc::iterator       iterator;
  typedef typename Alloc::const_iterator const_iterator;

public:
  /// default constructor
  NodeFactory()
  : Alloc(), m_NodeNum(0)
  { }

  /// destructor - release all controlled memory.
  virtual ~NodeFactory()
  { Alloc::clear(); }

  /// produce - produce a node, add it under control
  NodeType* produce() {
    ++m_NodeNum;
    NodeType* result = Alloc::allocate();
    Alloc::construct(result);
    return result;
  }

  /// size - the number of created nodes.
  unsigned int size() const
  { return m_NodeNum; }

  /// empty - is there any node under control?
  bool empty() const
  { return (0 == m_NodeNum); }
  
  /// delegate - get the control of chunks owned by the client
  //  after calling delegate(), client will renouce its control
  //  of memory space.
  void delegate(NodeFactory& pClient) {
    if (pClient.empty())
      return;

    if (Alloc::empty()) {
      replace(pClient);
      pClient.renounce();
      return;
    }

    // neither me nor client is empty
    concatenate(pClient);
    pClient.renounce();
  }

  // -----  iterators  ----- //
  iterator begin()             { return Alloc::begin(); }
  iterator end()               { return Alloc::end(); }
  const_iterator begin() const { return Alloc::begin(); }
  const_iterator end() const   { return Alloc::end(); }

private:
  /// renounce - give up the control of all chunks
  void renounce() {
    Alloc::m_pRoot = 0;
    Alloc::m_pCurrent = 0;
    Alloc::m_AllocatedNum = m_NodeNum = 0;
  }

  /// replace - be the agent of client.
  void replace(NodeFactory& pClient) {
    Alloc::m_pRoot = pClient.Alloc::m_pRoot;
    Alloc::m_pCurrent = pClient.Alloc::m_pCurrent;
    Alloc::m_AllocatedNum = pClient.Alloc::m_AllocatedNum;
    m_NodeNum = pClient.m_NodeNum;
  }

  /// concatenate - conncet two factories
  void concatenate(NodeFactory& pClient) {
    Alloc::m_pCurrent->next = pClient.Alloc::m_pRoot;
    Alloc::m_pCurrent = pClient.Alloc::m_pCurrent;
    Alloc::m_AllocatedNum += pClient.Alloc::m_AllocatedNum;
    m_NodeNum += pClient.m_NodeNum;
  }

private:
  unsigned int m_NodeNum;
};

} // namespace of mcld

#endif

