/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef BINTREE_H
#define BINTREE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <iterator>
#include <memory>
#include <set>

namespace mcld
{

//===----------------------------------------------------------------------===//
// Traits
template <class DataType>
struct NonconstTraits;

template <class DataType>
struct ConstTraits {
  typedef DataType                 value_type;
  typedef const DataType&          reference;
  typedef const DataType*          pointer;
  typedef ConstTraits<DataType>    ConstTraits;
  typedef NonConstTraits<DataType> NonConstTraits;
};

template <class DataType>
struct NonConstTraits {
  typedef DataType                 value_type;
  typedef DataType&                reference;
  typedef DataType*                pointer;
  typedef ConstTraits<DataType>    ConstTraits;
  typedef NonConstTraits<DataType> NonConstTraits;
};

//===----------------------------------------------------------------------===//
// Nodes
struct NodeBase
{
  NodeBase *left;
  NodeBase *right;
};

template<typename DataTy>
struct Node : public NodeBase
{
  DataTy* data;
};

//===----------------------------------------------------------------------===//
// iterator
namespace proxy
{
  template<size_t DIRECT>
  inline void move(NodeBase *X) {
    assert(0 && "not allowed");
  }
} // namespace of template proxy

struct IteratorBase
{
  enum Direct {
    Leftward,
    Rightward
  };
  typedef size_t               size_type;
  typedef ptrdiff_t            difference_type;
  typedef bidirectional_iterator_tag iterator_category;

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
} //namespace of template proxy

template<class DataType, class Traits>
struct TreeIterator : public IteratorBase
{
  typedef DataType                       value_type;
  typedef typename Traits::pointer       pointer;
  typedef typename Traits::reference     reference;

  typedef TreeIterator<DataType, Traits> Self;
  typedef typename Traits::NonConstTraits        NonConstTraits;
  typedef TreeIterator<DataType, NonConstTraits> iterator;
  typedef typename Traits::ConstTraits           ConstTraits;
  typedef TreeIterator<DataType, ConstTraits>    const_iterator;

  typedef bidirectional_iterator_tag           iterator_category;
  typedef Node<DataType>                 Node;
  typedef size_t                         size_type;
  typedef ptrdiff_t                      difference_type;


  TreeIterator()
    : IteratorBase(0) {}

  explicit TreeIterator(NodeBase* X)
    : IteratorBase(X) {}

  TreeIterator(const iterator &X)
    : IteratorBase(X.m_pNode) {}

  // -----  operators  ----- //
  reference operator*() const 
  { return static_cast<Node*>(m_pNode)->data; }

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
template<class DataType, class Alloc>
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

  void destroyNode(Node<DataType> *pNode) {
    m_TreeImple.NodeAllocType::deallocate(pNode, 1);
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
    m_TreeImple.NodeAllocType::clear();
  }
};

template<typename DataType, typename DataAlloc = std::allocator<DataType> >
class NodeAllocator
{
public:
  typedef typename        DataAlloc                          DataAllocType;
  typedef typename        NodeAllocator<DataType, DataAlloc> NodeAllocType;
  typedef size_t          size_type;
  typedef ptrdiff_t       difference_type;
  typedef DataType*       pointer;
  typedef const DataType* const_pointer;
  typedef DataType&       reference;
  typedef const DataType& const_reference;
  typedef DataType        value_type;
  typedef Node<DataType>  NodeType;

private:
  typedef std::multiset<NodeType*> NodeListType;
public:
  NodeAllocator() {
  }

  NodeAllocator(const NodeAllocator& pCopy) 
    : m_NodeList(pCopy.m_NodeList) {
  }

  ~NodeAllocator() {
     clear();
  }

  Node<DataType>* allocate(size_type N) {
    DataType *data = DataAlloc.allocate(1);
    Node<DataType> *result = new Node<DataType>();
    result->data = data;
    m_NodeList.insert(result);
    return result;
  }

  void deallocate(Node<DataType>*& pNode, size_type N) {
    DataAlloc.deallocate(pNode->data, 1);
    NodeListType::iterator position = m_NodeList.find(pNode);
    m_NodeList.erase(position);
    delete pNode;
    pNode = 0;
  }

  void clear() {
    NodeListType::iterator node;
    NodeListType::iterator nodeEnd = m_NodeList.end();
    for (node = m_NodeList.begin(); node!=nodeEnd; ++node) {
      DataAlloc.deallocate((*node)->data, 1);
      delete (*node);
    }
    m_NodeList.clear();
  }

  unsigned int size() const {
    return m_NodeList.size();
  }
private:
  NodeListType m_NodeList;
};

/** \class BinTree
 *  \brief An abstract data type of binary tree.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
template<class DataType, class Alloc = NodeAllocator<DataType> >
class BinTree : public BinTreeBase<DataType, Alloc>
{
public:
  typedef BinTree<DataType, Alloc>  Self;
  typedef Node<DataType>            Node;
  typedef NodeBase                  NodeBase;

public:
  typedef DataType value_type;
  typedef value_type* pointer;

  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  typedef typename TreeIterator<DataType, NonConstTraits<DataType> >  iterator;
  typedef typename TreeIterator<DataType, ConstTraits<DataType> >     const_iterator;

public:
  // -----  constructors and destructor  ----- //
  BinTree();
  BinTree(const BinTree& pCopy);
  ~BinTree();

  // -----  node operator  ----- //
  const_reference root() const;

  // ----- modifiers  ----- //
  /// join - create a leaf node and merge it in the tree.
  //  @param DIRECT the direction of the connecting edge of the parent node.
  //  @param position the parent node
  //  @param value the value being pushed.
  template<size_t DIRECT>
  BinTree& join(iterator position, const DataType& value) {
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

  // -----  iterators  ----- //
  iterator begin() {
  }

  iterator end() {
  }

  const_iterator begin() const {
  }

  const_iterator end() const {
  }
};

} // namespace of mcld

//===----------------------------------------------------------------------===//
// implementation
template<class DataType, class Alloc>
BinTree<DataType, Alloc>::BinTree()
  : BinTreeBase() {
}

template<class DataType, class Alloc>
BinTree<DataType, Alloc>::BinTree(const BinTree<DataType, Alloc>& pCopy)
  : BinTreeBase(pCopy) {
}

template<class DataType, class Alloc>
BinTree<DataType, Alloc>::~BinTree()
{
}

template<class DataType, class Alloc>
BinTree<DataType, Alloc>::const_reference BinTree<DataType, Alloc>::root() const
{
  BinTreeBase::m_TreeImpl
}

template<class DataType, class Alloc>
void BinTree<DataType, Alloc>::clear()
{
}

template<class DataType, class Alloc>
unsigned int BinTree<DataType, Alloc>::size() const
{
}

template<class DataType, class Alloc>
bool BinTree<DataType, Alloc>::empty() const
{
}

#endif

