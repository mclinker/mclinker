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
#include <mcld/ADT/TreeAllocator.h>

namespace mcld
{

//===----------------------------------------------------------------------===//
// iterator
namespace proxy
{
  template<size_t DIRECT>
  inline void move(NodeBase *&X) {
    assert(0 && "not allowed");
  }

  template<size_t DIRECT>
  inline void hook(NodeBase *X, NodeBase *Y) {
    assert(0 && "not allowed");
  }
} // namespace of template proxy

/** \class TreeIteratorBase
 *  \brief TreeIteratorBase provides the motion functions on a binary tree.
 *
 *  @see TreeIterator
 */
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
  inline void move<TreeIteratorBase::Leftward>(NodeBase *&X) {
    X = X->left;
  }

  template<>
  inline void move<TreeIteratorBase::Rightward>(NodeBase *&X) {
    X = X->right;
  }

  template<>
  inline void hook<TreeIteratorBase::Leftward>(NodeBase *X, NodeBase *Y) {
    X->left = Y;
  }

  template<>
  inline void hook<TreeIteratorBase::Rightward>(NodeBase* X, NodeBase* Y) {
    X->right = Y;
  }

} //namespace of template proxy

template<class DataType>
class BinaryTree;

/** \class TreeIterator
 *  \brief TreeIterator provides full functions of binary tree's iterator.
 *
 *  TreeIterator is designed to compatible with STL iterators.
 *  TreeIterator is bi-directional. Incremental direction means to move
 *  rightward, and decremental direction is leftward.
 *
 *  @see TreeIteratorBase
 */
template<class DataType, class Traits>
struct TreeIterator : public TreeIteratorBase
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
    : TreeIteratorBase(0) {}

  TreeIterator(const iterator &X)
    : TreeIteratorBase(X.m_pNode) {}

  ~TreeIterator() {}

  // -----  operators  ----- //
  reference operator*() const 
  { return static_cast<node_type*>(m_pNode)->data; }

  Self& operator++() {
    this->move<TreeIteratorBase::Rightward>();
    return *this;
  }

  Self operator++(int) {
    Self tmp = *this;
    this->move<TreeIteratorBase::Rightward>();
    return tmp;
  }

  Self& operator--() {
    this->move<TreeIteratorBase::Leftward>();
    return *this;
  }

  Self operator--(int) {
    Self tmp = *this;
    this->move<TreeIteratorBase::Leftward>();
    return tmp;
  }
private:
  friend class BinaryTree<DataType>;

  explicit TreeIterator(NodeBase* X)
    : TreeIteratorBase(X) {}
};

//===----------------------------------------------------------------------===//
// Tree

/** \class BinaryTreeBase
 *  \brief BinaryTreeBase gives root node and memory management.
 *
 *  The memory management of nodes in is hidden by BinaryTreeBase.
 *  BinaryTreeBase also provides the basic functions for merging a tree and 
 *  inserton of a node.
 * 
 *  @see BinaryTree
 */
template<class DataType>
class BinaryTreeBase
{
public:
  typedef Node<DataType> NodeType;
protected:
  /// TreeImpl - TreeImpl records the root node and the number of nodes
  //
  //    +---> Root(end) <---+
  //    |        |right     |
  //    |      begin        |
  //    |     /     \       |
  //    |  Left     Right   |
  //    +---/         \-----+
  //     
  class TreeImpl : public NodeFactory<DataType>
  {
  public:
    typedef typename NodeFactory<DataType>::iterator       iterator;
    typedef typename NodeFactory<DataType>::const_iterator const_iterator;

  public:
    NodeBase node;

  public:
    TreeImpl()
      : NodeFactory<DataType>() {
      node.left = node.right = &node;
    }

    ~TreeImpl()
    { }

    /// summon - change the final edges of pClient to our root
    void summon(TreeImpl& pClient) {
      iterator data;
      iterator dEnd = pClient.end();
      for (data = pClient.begin(); data!=dEnd; data.next() ) {
        if (data.get()->left == &pClient.node)
          data.get()->left = &node;
        if (data.get()->right == &pClient.node)
          data.get()->right = &node;
      }
    }
  };

protected:
  /// m_Root is a special object who responses:
  //  - the pointer of root
  //  - the simple factory of nodes.
  TreeImpl m_Root;

protected:
  NodeType *createNode() {
    NodeType *result = m_Root.produce();
    result->left = result->right = m_Root.node;
    return result;
  }

  void destroyNode(NodeType *pNode) {
    pNode->left = pNode->right = 0;
    pNode->data = 0;
    m_Root.deallocate(pNode);
  }

public:
  BinaryTreeBase()
  : m_Root()
  { }

  BinaryTreeBase(const BinaryTreeBase& pCopy)
  : m_Root(pCopy.m_Root)
  { }

  virtual ~BinaryTreeBase()
  { }

  size_t size() const {
    return m_Root.size();
  }

  bool empty() const {
    return m_Root.empty();
  }

protected:
  void clear() {
    m_Root.clear();
  }
};

/** \class BinaryTree
 *  \brief An abstract data type of binary tree.
 *
 *  @see mcld::InputTree
 */
template<class DataType>
class BinaryTree : public BinaryTreeBase<DataType>
{
public:
  typedef size_t             size_type;
  typedef ptrdiff_t          difference_type;
  typedef DataType           value_type;
  typedef value_type*        pointer;
  typedef value_type&        reference;
  typedef const value_type*  const_pointer;
  typedef const value_type&  const_reference;

  typedef BinaryTree<DataType>  Self;
  typedef TreeIterator<value_type, NonConstTraits<value_type> > iterator;
  typedef TreeIterator<value_type, ConstTraits<value_type> >    const_iterator;

protected:
  typedef Node<value_type> node_type;

public:
  // -----  constructors and destructor  ----- //
  BinaryTree()
  : BinaryTreeBase<DataType>()
  { }

  BinaryTree(const BinaryTree& pCopy)
  : BinaryTreeBase<DataType>(pCopy.m_Root)
  { }

  ~BinaryTree() {
  }

  // -----  iterators  ----- //
  iterator root() {
    return iterator(&(BinaryTreeBase<DataType>::m_Root.node));
  }

  const_iterator root() const {
    return const_iterator(&(BinaryTreeBase<DataType>::m_Root.node));
  }

  iterator begin() {
    return iterator(BinaryTreeBase<DataType>::m_Root.node.right);
  }

  iterator end() {
    return iterator(BinaryTreeBase<DataType>::m_Root.node.left);
  }

  const_iterator begin() const {
    return const_iterator(BinaryTreeBase<DataType>::m_Root.node.right);
  }

  const_iterator end() const {
    return const_iterator(BinaryTreeBase<DataType>::m_Root.node.left);
  }

  // ----- modifiers  ----- //
  /// join - create a leaf node and merge it in the tree.
  //  @param DIRECT the direction of the connecting edge of the parent node.
  //  @param position the parent node
  //  @param value the value being pushed.
  template<size_t DIRECT>
  BinaryTree& join(iterator position, const DataType& value) {
    node_type *node = createNode(value);
    proxy::hook<DIRECT>(position.m_pNode, node);
    return *this;
  }

  /// merge - merge the tree
  //  @param DIRECT the direction of the connecting edge of the parent node.
  //  @param position the parent node
  //  @param the tree being joined.
  //  @return the joined tree
  template<size_t DIRECT>
  BinaryTree& merge(iterator position, BinaryTree& pTree) {
    if (!pTree.empty()) {
      proxy::hook<DIRECT>(position.m_pNode, pTree.m_Root.node.right);
      BinaryTreeBase<DataType>::m_Root.summon(
                                   pTree.BinaryTreeBase<DataType>::m_Root);
      BinaryTreeBase<DataType>::m_Root.delegate(pTree.m_Root);
      pTree.m_Root.node.left = pTree.m_Root.node.right = &pTree.m_Root.node;
    }
    return *this;
  }
};

} // namespace of mcld

#endif

