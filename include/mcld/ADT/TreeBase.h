/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_TREE_BASE_H
#define MCLD_TREE_BASE_H
#include <mcld/ADT/TypeTraits.h>

namespace mcld
{

class NodeBase
{
public:
  NodeBase *left;
  NodeBase *right;

public:
  NodeBase()
  : left(0), right(0)
  { }
};

namespace proxy
{
  template<size_t DIRECT>
  inline void move(NodeBase *&X)
  { assert(0 && "not allowed"); }

  template<size_t DIRECT>
  inline void hook(NodeBase *X, const NodeBase *Y)
  { assert(0 && "not allowed"); }

} // namespace of template proxy

struct TreeIteratorBase
{
public:
  enum Direct {
    Leftward,
    Rightward
  };

  typedef size_t                          size_type;
  typedef ptrdiff_t                       difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

public:
  NodeBase* m_pNode;

public:
  TreeIteratorBase(NodeBase *X)
    : m_pNode(X) {
  }

  template<typename DIRECT>
  inline void move() {
    proxy::move<DIRECT>(m_pNode);
  }

  bool operator==(const TreeIteratorBase& y) const
  { return this->m_pNode == y.m_pNode; }

  bool operator!=(const TreeIteratorBase& y) const
  { return this->m_pNode != y.m_pNode; }
};

namespace proxy
{
  template<>
  inline void move<TreeIteratorBase::Leftward>(NodeBase *&X) 
  { X = X->left; }

  template<>
  inline void move<TreeIteratorBase::Rightward>(NodeBase *&X)
  { X = X->right; }

  template<>
  inline void hook<TreeIteratorBase::Leftward>(NodeBase *X, const NodeBase *Y)
  { X->left = const_cast<NodeBase*>(Y); }

  template<>
  inline void hook<TreeIteratorBase::Rightward>(NodeBase* X, const NodeBase* Y)
  { X->right = const_cast<NodeBase*>(Y); }

} //namespace of template proxy

template<typename DataType>
class Node : public NodeBase
{
public:
  typedef DataType        value_type;

public:
  value_type* data;

public:
  Node()
  : NodeBase(), data(0)
  { }

  Node(const value_type& pValue)
  : NodeBase(), data(&pValue)
  { }

};

} // namespace of mcld

#endif

