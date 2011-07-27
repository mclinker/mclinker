/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_BINARY_TREE_H
#define MCLD_BINARY_TREE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <iterator>
#include <memory>
#include <set>
#include <mcld/Support/TreeAllocator.h>

namespace mcld
{

//===----------------------------------------------------------------------===//
// iterator
namespace proxy
{
  template<size_t DIRECT>
  inline void move(NodeBase *X) {
    assert(0 && "not allowed");
  }

  template<size_t DIRECT>
  inline void hook(NodeBase* X, NodeBase* Y) {
    assert(0 && "not allowed");
  }
} // namespace of template proxy

struct IteratorBase
{
  enum Direct {
    Leftward,
    Rightward
  };
  typedef size_t                          size_type;
  typedef ptrdiff_t                       difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

  NodeBase* m_pNode;

  IteratorBase(NodeBase *X)
    : m_pNode(X) {
  }

  template<typename DIRECT>
  inline void move() {
    proxy::move<DIRECT>(m_pNode);
  }

  bool operator==(const IteratorBase& y) const
  { return this->m_pNode == y.m_pNode; }

  bool operator!=(const IteratorBase& y) const
  { return this->m_pNode != y.m_pNode; }
};

namespace proxy
{
  template<>
  inline void move<IteratorBase::Leftward>(NodeBase* X) {
    X = X->left;
  }

  template<>
  inline void move<IteratorBase::Rightward>(NodeBase* X) {
    X = X->right;
  }

  template<>
  inline void hook<IteratorBase::Leftward>(NodeBase* X, NodeBase* Y) {
    X->left = Y;
  }

  template<>
  inline void hook<IteratorBase::Rightward>(NodeBase* X, NodeBase* Y) {
    X->right = Y;
  }

} //namespace of template proxy

template<class DataType, class Traits>
struct TreeIterator : public IteratorBase
{
public:
  typedef DataType                       value_type;
  typedef Traits                         traits;
  typedef typename traits::pointer       pointer;
  typedef typename traits::reference     reference;

  typedef TreeIterator<value_type, Traits>          Self;
  typedef Node<value_type>                          node_type;

  typedef typename traits::nonconst_traits          nonconst_traits;
  typedef TreeIterator<value_type, nonconst_traits> iterator;
  typedef typename traits::const_traits             const_traits;
  typedef TreeIterator<value_type, const_traits>    const_iterator;
  typedef std::bidirectional_iterator_tag           iterator_category;
  typedef size_t                                    size_type;
  typedef ptrdiff_t                                 difference_type;

public:
  TreeIterator()
    : IteratorBase(0) {}

  explicit TreeIterator(NodeBase* X)
    : IteratorBase(X) {}

  TreeIterator(const iterator &X)
    : IteratorBase(X.m_pNode) {}

  // -----  operators  ----- //
  reference operator*() const 
  { return static_cast<node_type*>(m_pNode)->data; }

  Self& operator++() {
    this->move<IteratorBase::Rightward>();
    return *this;
  }

  Self operator++(int) {
    Self tmp = *this;
    this->move<IteratorBase::Rightward>();
    return tmp;
  }

  Self& operator--() {
    this->move<IteratorBase::Leftward>();
    return *this;
  }

  Self operator--(int) {
    Self tmp = *this;
    this->move<IteratorBase::Leftward>();
    return tmp;
  }
};

//===----------------------------------------------------------------------===//
// Tree
template<class DataType, class Alloc = NodeAllocator<DataType> >
class BinTreeBase
{
protected:
  typedef typename Alloc::NodeAllocType NodeAllocType;
  typedef typename Alloc::DataAllocType DataAllocType;

  struct TreeImpl : public NodeAllocType
  {
    NodeBase node;

    TreeImpl()
      : NodeAllocType(), node() {
    }

    TreeImpl(const NodeAllocType& pCopy)
      : NodeAllocType(pCopy), node() {
    }
  };

protected:
  /// m_TreeImpl is a special object who responses:
  //  - the pointer of root and end
  //  - the simple factory of nodes.
  TreeImpl m_TreeImpl;

protected:
  Node<DataType> *createNode() {
    return m_TreeImpl.NodeAllocType::allocate(1);
  }

  Node<DataType> *createNode(const DataType &pValue) {
    Node<DataType> *result = m_TreeImpl.NodeAllocType::allocate(1);
    m_TreeImpl.NodeAllocType::construct(result, pValue);
    return result;
  }

  void destroyNode(Node<DataType> *pNode) {
    m_TreeImpl.NodeAllocType::deallocate(pNode, 1);
  }

public:
  NodeAllocType& getNodeAlloc() {
    return *static_cast<NodeAllocType*>(&(this->m_TreeImpl));
  }

  const NodeAllocType& getNodeAlloc() const {
    return *static_cast<const NodeAllocType*>(&(this->m_TreeImpl));
  }

public:
  BinTreeBase()
    : m_TreeImpl()
  { init(); }

  BinTreeBase(const NodeAllocType& pAlloc)
    : m_TreeImpl(pAlloc)
  { init(); }

 ~BinTreeBase() {
    clear();
  }
protected:
  void init() {
    this->m_TreeImpl.node.left = &this->m_TreeImpl.node;
    this->m_TreeImpl.node.right = &this->m_TreeImpl.node;
  }

  void clear() {
    m_TreeImpl.NodeAllocType::clear();
  }
};

/** \class BinTree
 *  \brief An abstract data type of binary tree.
 *
 *  @see mcld::InputTree
 */
template<class DataType>
class BinTree : public BinTreeBase<DataType>
{
public:
  typedef BinTree<DataType>  Self;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef DataType value_type;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type* const_pointer;
  typedef const value_type& const_reference;

  typedef TreeIterator<value_type, NonConstTraits<value_type> > iterator;
  typedef TreeIterator<value_type, ConstTraits<value_type> >    const_iterator;

protected:
  typedef Node<value_type>            node_type;

public:
  // -----  constructors and destructor  ----- //
  BinTree();
  BinTree(const BinTree& pCopy);
  virtual ~BinTree();

  // -----  iterators  ----- //
  iterator root() {
  }

  const_iterator root() const {
  }

  iterator begin() {
  }

  iterator end() {
  }

  const_iterator begin() const {
  }

  const_iterator end() const {
  }

  // ----- modifiers  ----- //
  /// join - create a leaf node and merge it in the tree.
  //  @param DIRECT the direction of the connecting edge of the parent node.
  //  @param position the parent node
  //  @param value the value being pushed.
  template<size_t DIRECT>
  BinTree& join(iterator position, const DataType& value) {
    node_type *node = createNode(value);
    proxy::hook<DIRECT>(position.IteratorBase::m_pNode, node);
  }

  /// merge - merge the tree
  //  @param DIRECT the direction of the connecting edge of the parent node.
  //  @param position the parent node
  //  @param the tree being joined.
  //  @return the joined tree
  template<size_t DIRECT>
  BinTree& merge(iterator position, const BinTree& pTree) {
  }

  /// clear - clear all nodes in the tree
  void clear();

  // -----  observers  ----- //
  unsigned int size() const;
  bool empty() const;
};

} // namespace of mcld

//===----------------------------------------------------------------------===//
// implementation
template<class DataType>
mcld::BinTree<DataType>::BinTree()
  : BinTreeBase<DataType>() {
}

template<class DataType>
mcld::BinTree<DataType>::BinTree(const BinTree<DataType>& pCopy)
  : BinTreeBase<DataType>(pCopy) {
}

template<class DataType>
mcld::BinTree<DataType>::~BinTree()
{
}

template<class DataType>
void mcld::BinTree<DataType>::clear()
{
  BinTreeBase<DataType>::clear();
  BinTreeBase<DataType>::init();
}

template<class DataType>
unsigned int mcld::BinTree<DataType>::size() const
{
  return BinTreeBase<DataType>::m_TreeImpl.size();
}

template<class DataType>
bool mcld::BinTree<DataType>::empty() const
{
  return BinTreeBase<DataType>::m_TreeImpl.empty();
}

#endif

