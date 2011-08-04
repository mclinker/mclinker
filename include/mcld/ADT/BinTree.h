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
#include <stack>
#include <queue>
#include <mcld/ADT/Uncopyable.h>
#include <mcld/ADT/TreeAllocator.h>
#include <iostream>
using namespace std;
namespace mcld
{

template<class DataType>
class BinaryTree;

struct TraversalIteratorBase
{
public:
  typedef size_t                          size_type;
  typedef ptrdiff_t                       difference_type;
  typedef std::forward_iterator_tag       iterator_category;

public:
  NodeBase* m_pNode;

public:
  TraversalIteratorBase(NodeBase *X)
    : m_pNode(X) {
  }

  virtual ~TraversalIteratorBase(){};

  bool hasRightChild() const
  { return ((m_pNode->right) != (m_pNode->right->right)); }

  bool hasLeftChild() const
  { return ((m_pNode->left) != (m_pNode->left->right)); }

  bool operator==(const TraversalIteratorBase& y) const
  { return this->m_pNode == y.m_pNode; }

  bool operator!=(const TraversalIteratorBase& y) const
  { return this->m_pNode != y.m_pNode; }
};

struct DFSIterator : public TraversalIteratorBase
{
public:
  DFSIterator(NodeBase *X)
    : TraversalIteratorBase(X) {
  }

  void advance() {
    if(m_Stack.empty())
    {
      if(hasRightChild())
        m_Stack.push(m_pNode->right);
      if(hasLeftChild())
        m_Stack.push(m_pNode->left);
      if(m_Stack.empty())
        m_pNode = m_pNode->right;
    }
    if(!m_Stack.empty())
    {
      m_pNode = m_Stack.top();
      m_Stack.pop();
      if(hasRightChild())
        m_Stack.push(m_pNode->right);
      if(hasLeftChild())
        m_Stack.push(m_pNode->left);
    }
  }

private:
    std::stack<NodeBase *> m_Stack;
};

struct BFSIterator : public TraversalIteratorBase
{
public:
  BFSIterator(NodeBase *X)
    : TraversalIteratorBase(X) {
    }

  virtual ~BFSIterator(){};

  void advance() { 
    if(m_Queue.empty())
    {
      if(hasLeftChild())
        m_Queue.push(m_pNode->left);
      if(hasRightChild())
        m_Queue.push(m_pNode->right);
      if(m_Queue.empty())
        m_pNode = m_pNode->right;
    }
    if(!m_Queue.empty())
    {
      m_pNode = m_Queue.front();
      m_Queue.pop();
      if(hasLeftChild())
        m_Queue.push(m_pNode->left);
      if(hasRightChild())
        m_Queue.push(m_pNode->right);
    }
  }

private:
    std::queue<NodeBase *> m_Queue;
};

template<class DataType, class Traits, class IteratorType>
struct VarietyIterator : public IteratorType
{
  typedef DataType                       value_type;
  typedef Traits                         traits;
  typedef typename traits::pointer       pointer;
  typedef typename traits::reference     reference;
  
  typedef VarietyIterator<value_type, Traits, IteratorType>           Self;
  typedef Node<value_type>                                            node_type;
  typedef typename traits::nonconst_traits                            nonconst_traits;
  typedef VarietyIterator<value_type, nonconst_traits, IteratorType>  iterator;
  typedef typename traits::const_traits                               const_traits;
  typedef VarietyIterator<value_type, const_traits, IteratorType>     const_iterator;
  typedef std::forward_iterator_tag                                   iterator_category;
  typedef size_t                                                      size_type;
  typedef ptrdiff_t                                                   difference_type;

public:
  VarietyIterator()
    : IteratorType(0) {}

  VarietyIterator(const iterator &X)
    : IteratorType(X.m_pNode) {}

  ~VarietyIterator() {}

  // -----  operators  ----- //
  pointer operator*() const 
  { return static_cast<node_type*>(IteratorType::m_pNode)->data; }

  bool isRoot() const
  { return (IteratorType::m_pNode->right == IteratorType::m_pNode); }

  bool hasData() const 
  { return (!isRoot() && (0 != static_cast<node_type*>(IteratorType::m_pNode)->data)); }

  Self& operator++() {  
    IteratorType::advance();
    return *this;
  }

  Self operator++(int) {
    Self tmp = *this;
    IteratorType::advance();
    return tmp;
  }

  explicit VarietyIterator(NodeBase* X)
    : IteratorType(X) {}

};

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
  pointer operator*() const 
  { return static_cast<node_type*>(m_pNode)->data; }

  bool isRoot() const
  { return (m_pNode->right == m_pNode); }

  bool hasData() const 
  { return (!isRoot() && (0 != static_cast<node_type*>(m_pNode)->data)); }

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

  explicit TreeIterator(NodeBase* X)
    : TreeIteratorBase(X) {}
};

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
class BinaryTreeBase : private Uncopyable
{
public:
  typedef Node<DataType> NodeType;
protected:
  /// TreeImpl - TreeImpl records the root node and the number of nodes
  //
  //    +---> Root(end) <---+
  //    |        |left      |
  //    |      begin        |
  //    |     /     \       |
  //    |  Left     Right   |
  //    +---/         \-----+
  //     
  class TreeImpl : public NodeFactory<DataType>
  {
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
      if (this == &pClient)
        return;

      iterator data;
      iterator dEnd = pClient.end();
      for (data = pClient.begin(); data!=dEnd; ++data ) {
        if ((*data).left == &pClient.node)
          (*data).left = &node;
        if ((*data).right == &pClient.node)
          (*data).right = &node;
      }
    }
  }; // TreeImpl

protected:
  /// m_Root is a special object who responses:
  //  - the pointer of root
  //  - the simple factory of nodes.
  TreeImpl m_Root;

protected:
  NodeType *createNode() {
    NodeType *result = m_Root.produce();
    result->left = result->right = &m_Root.node;
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

  typedef VarietyIterator<value_type, NonConstTraits<value_type>, DFSIterator> dfs_iterator;
  typedef VarietyIterator<value_type, ConstTraits<value_type>, DFSIterator>    dfs_const_iterator;
  typedef VarietyIterator<value_type, NonConstTraits<value_type>, BFSIterator> bfs_iterator;
  typedef VarietyIterator<value_type, ConstTraits<value_type>, BFSIterator>    bfs_const_iterator;

protected:
  typedef Node<value_type> node_type;

public:
  // -----  constructors and destructor  ----- //
  BinaryTree()
  : BinaryTreeBase<DataType>()
  { }

  ~BinaryTree() {
  }

  // -----  iterators  ----- //
  bfs_iterator bfs_begin()
  { return bfs_iterator(BinaryTreeBase<DataType>::m_Root.node.left); }

  bfs_iterator bfs_end()
  { return bfs_iterator(BinaryTreeBase<DataType>::m_Root.node.right); }

  bfs_const_iterator bfs_const_begin() const
  { return bfs_const_iterator(BinaryTreeBase<DataType>::m_Root.node.left); }

  bfs_const_iterator bfs_const_end() const
  { return bfs_const_iterator(BinaryTreeBase<DataType>::m_Root.node.right); }

  dfs_iterator dfs_begin()
  { return dfs_iterator(BinaryTreeBase<DataType>::m_Root.node.left); }

  dfs_iterator dfs_end()
  { return dfs_iterator(BinaryTreeBase<DataType>::m_Root.node.right); }

  dfs_const_iterator dfs_const_begin() const
  { return dfs_const_iterator(BinaryTreeBase<DataType>::m_Root.node.left); }

  dfs_const_iterator dfs_const_end() const
  { return dfs_const_iterator(BinaryTreeBase<DataType>::m_Root.node.right); }

  iterator root()
  { return iterator(&(BinaryTreeBase<DataType>::m_Root.node)); }

  const_iterator root() const
  { return const_iterator(&(BinaryTreeBase<DataType>::m_Root.node)); }

  iterator begin()
  { return iterator(BinaryTreeBase<DataType>::m_Root.node.left); }

  iterator end()
  { return iterator(BinaryTreeBase<DataType>::m_Root.node.right); }

  const_iterator begin() const
  { return const_iterator(BinaryTreeBase<DataType>::m_Root.node.left); }

  const_iterator end() const
  { return const_iterator(BinaryTreeBase<DataType>::m_Root.node.right); }

  // ----- modifiers  ----- //
  /// join - create a leaf node and merge it in the tree.
  //  This version of join determines the direction on compilation time.
  //  @param DIRECT the direction of the connecting edge of the parent node.
  //  @param position the parent node
  //  @param value the value being pushed.
  template<size_t DIRECT>
  BinaryTree& join(iterator position, const DataType& value) {
    node_type *node = BinaryTreeBase<DataType>::createNode();
    node->data = const_cast<DataType*>(&value);
    if (position.isRoot())
      proxy::hook<TreeIteratorBase::Leftward>(position.m_pNode,
                          const_cast<const node_type*>(node));
    else
      proxy::hook<DIRECT>(position.m_pNode,
                          const_cast<const node_type*>(node));
    return *this;
  }

  /// merge - merge the tree
  //  @param DIRECT the direction of the connecting edge of the parent node.
  //  @param position the parent node
  //  @param the tree being joined.
  //  @return the joined tree
  template<size_t DIRECT>
  BinaryTree& merge(iterator position, BinaryTree& pTree) {
    if (this == &pTree)
      return *this;

    if (!pTree.empty()) {
      proxy::hook<DIRECT>(position.m_pNode,
                        const_cast<const NodeBase*>(pTree.m_Root.node.right));
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

