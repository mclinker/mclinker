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
  inline void move(NodeBase *&X) {
    assert(0 && "not allowed");
  }

  template<size_t DIRECT>
  inline void hook(NodeBase *X, NodeBase *Y) {
    assert(0 && "not allowed");
  }
} // namespace of template proxy

struct IteratorBase
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
  inline void move<IteratorBase::Leftward>(NodeBase *&X) {
    X = X->left;
  }

  template<>
  inline void move<IteratorBase::Rightward>(NodeBase *&X) {
    X = X->right;
  }

  template<>
  inline void hook<IteratorBase::Leftward>(NodeBase *X, NodeBase *Y) {
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

  TreeIterator(const iterator &X)
    : IteratorBase(X.m_pNode) {}

  ~TreeIterator() {}

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
private:
  template<class DataType>
  friend class BinTree<DataType>;

  explicit TreeIterator(NodeBase* X)
    : IteratorBase(X) {}
};

//===----------------------------------------------------------------------===//
// Tree
template<class DataType, class Alloc = NodeFactory<DataType> >
class BinTreeBase
{
protected:
  typedef typename Alloc::NodeFactory     node_factory;
  typedef typename Alloc::DataFactory     data_factory;
  typedef typename node_factory::NodeType node_type;

  /// TreeImpl - TreeImpl records the root node and the number of nodes
  //
  //    +---> Root(end) <---+
  //    |        |right     |
  //    |      begin        |
  //    |     /     \       |
  //    |  Left     Right   |
  //    +---/         \-----+
  //     
  struct TreeImpl : public node_factory
  {
  public:
    NodeBase node;

  public:
    TreeImpl()
      : NodeAllocType() {
      node.left = node.right = &node;
    }

    TreeImpl(const NodeAllocType& pCopy)
      : NodeAllocType(pCopy), node(pCopy.node) {
    }

    void delegate(TreeImpl& pTree) {
      node_factory::delegate(node, pTree.node_factory);
    }
  };

protected:
  /// m_Root is a special object who responses:
  //  - the pointer of root
  //  - the simple factory of nodes.
  TreeImpl m_Root;

protected:
  NodeType *createNode(const DataType &pValue) {
    NodeType *result = m_Root.node_factory::allocate();
    m_Root.node_factory::construct(result, pValue);
    result->left = result->right = m_Root.node;
    return result;
  }

  void destroyNode(NodeType *pNode) {
    m_Root.node_factory::deallocate(pNode);
  }

public:
  BinTreeBase() : m_Root()
  { }

  BinTreeBase(const BinTreeBase& pCopy) : m_Root(pCopy.m_Root)
  { }

  virtual ~BinTreeBase()
  { clear(); }

  size_t size() const {
    return m_Root.size();
  }

  bool empty() const {
    return m_Root.empty();
  }

protected:
  void clear() {
    m_Root.node_factory::clear();
  }
};

/** \class BinTree
 *  \brief An abstract data type of binary tree.
 *
 *  @see mcld::InputTree
 */
template<class DataType, class Alloc = NodeFactory<DataType> >
class BinTree : public BinTreeBase<DataType, Alloc>
{
public:
  typedef size_t             size_type;
  typedef ptrdiff_t          difference_type;
  typedef DataType           value_type;
  typedef value_type*        pointer;
  typedef value_type&        reference;
  typedef const value_type*  const_pointer;
  typedef const value_type&  const_reference;

  typedef BinTree<DataType>  Self;
  typedef TreeIterator<value_type, NonConstTraits<value_type> > iterator;
  typedef TreeIterator<value_type, ConstTraits<value_type> >    const_iterator;

protected:
  typedef Node<value_type> node_type;

public:
  // -----  constructors and destructor  ----- //
  BinTree()
  : BinTreeBase()
  { }

  BinTree(const BinTree& pCopy)
  : BinTreeBase(pCopy.m_Root)
  { }

  ~BinTree() {
  }

  // -----  iterators  ----- //
  iterator root() {
    return iterator(&(m_Root.node));
  }

  const_iterator root() const {
    return const_iterator(&(m_Root.node));
  }

  iterator begin() {
    return iterator(m_Root.node.right);
  }

  iterator end() {
    return iterator(m_Root.node.left);
  }

  const_iterator begin() const {
    return const_iterator(m_Root.node.right);
  }

  const_iterator end() const {
    return const_iterator(m_Root.node.left);
  }

  // ----- modifiers  ----- //
  /// join - create a leaf node and merge it in the tree.
  //  @param DIRECT the direction of the connecting edge of the parent node.
  //  @param position the parent node
  //  @param value the value being pushed.
  template<size_t DIRECT>
  BinTree& join(iterator position, const DataType& value) {
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
  BinTree& merge(iterator position, BinTree& pTree) {
    if (!pTree.empty()) {
      proxy::hook<DIRECT>(position.m_pNode, pTree.m_Root.node.right);
      m_Root.delegate(pTree.m_Root);
    }
    return *this;
  }
};

} // namespace of mcld

#endif

