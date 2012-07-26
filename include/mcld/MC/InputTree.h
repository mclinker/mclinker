//===- InputTree.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_INPUT_TREE_H
#define MCLD_INPUT_TREE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/ADT/BinTree.h"
#include "mcld/ADT/TypeTraits.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/InputFactory.h"
#include "mcld/Support/FileSystem.h"

#include <string>


namespace mcld
{

/** \class template<typename Traits, typename Iterator> PolicyIterator<mcld::Input>
 *  \brief PolicyIterator<mcld::Input> is a partially specific PolicyIterator
 */
template<typename Traits, typename IteratorType>
class PolicyIterator<mcld::Input, Traits, IteratorType> : public PolicyIteratorBase<Input, Traits, IteratorType>
{
public:
  typedef PolicyIterator<Input, Traits, IteratorType> Self;
  typedef PolicyIteratorBase<Input, Traits, IteratorType> Base;
  typedef PolicyIterator<Input, typename Traits::nonconst_traits, IteratorType> iterator;
  typedef PolicyIterator<Input, typename Traits::const_traits, IteratorType>    const_iterator;

public:
  PolicyIterator()
    : Base() {}

  PolicyIterator(const iterator &X)
    : Base(X.m_pNode) {}

  explicit PolicyIterator(NodeBase* X)
    : Base(X) {}

  virtual ~PolicyIterator() {}

  bool isGroup() const
  { return !Base::hasData(); }

  Self& operator++() {
    IteratorType::advance();
    if (isGroup())
      IteratorType::advance();
    return *this;
  }

  Self operator++(int) {
    Self tmp(*this);
    IteratorType::advance();
    if (isGroup())
      IteratorType::advance();
    return tmp;
  }
};

/** \class InputTree
 *  \brief InputTree is the input tree to contains all inputs from the
 *  command line.
 *
 *  InputTree, of course, is uncopyable.
 *
 *  @see Input
 */
class InputTree : public BinaryTree<Input>
{
private:
  typedef BinaryTree<Input> BinTreeTy;

public:
  enum Direction {
    Inclusive  = TreeIteratorBase::Leftward,
    Positional = TreeIteratorBase::Rightward
  };

  typedef BinaryTree<Input>::iterator       iterator;
  typedef BinaryTree<Input>::const_iterator const_iterator;

public:
  /** \class Mover
   *  \brief Mover provides the interface for moving iterator forward.
   *
   *  Mover is a function object (functor). @ref Mover::move moves
   *  iterator forward in certain direction. @ref Mover::connect
   *  connects two nodes of the given iterators togather.
   */
  struct Mover {
    virtual ~Mover() {}
    virtual void connect(TreeIteratorBase& pFrom, const TreeIteratorBase& pTo) const = 0;
    virtual void move(TreeIteratorBase& pNode) const = 0;
  };

  /** \class Succeeder
   *  \brief class Succeeder moves the iterator afterward.
   */
  struct Succeeder : public Mover {
    virtual void connect(TreeIteratorBase& pFrom, const TreeIteratorBase& pTo) const {
      proxy::hook<Positional>(pFrom.m_pNode, pTo.m_pNode);
    }

    virtual void move(TreeIteratorBase& pNode) const {
      pNode.move<Positional>();
    }
  };

  /** \class Includer
   *  \brief class Includer moves the iterator downward.
   */
  struct Includer : public Mover {
    virtual void connect(TreeIteratorBase& pFrom, const TreeIteratorBase& pTo) const {
      proxy::hook<Inclusive>(pFrom.m_pNode, pTo.m_pNode);
    }

    virtual void move(TreeIteratorBase& pNode) const {
      pNode.move<Inclusive>();
    }
  };

public:
  static Succeeder Afterward;
  static Includer  Downward;

public:

  using BinTreeTy::merge;

  InputTree(InputFactory& pInputFactory);
  ~InputTree();

  // -----  modify  ----- //
  /// insert - create a leaf node and merge it in the tree.
  //  This version of join determines the direction at run time.
  //  @param pRoot  position the parent node
  //  @param pMover the direction of the connecting edge of the parent node.
  template<size_t DIRECT>
  InputTree& insert(TreeIteratorBase pRoot,
                    const std::string& pNamespec,
                    const sys::fs::Path& pPath,
                    unsigned int pType = Input::Unknown);

  template<size_t DIRECT>
  InputTree& enterGroup(TreeIteratorBase pRoot);

  template<size_t DIRECT>
  InputTree& insert(TreeIteratorBase pRoot,
                    const Input& pInput);

  InputTree& merge(TreeIteratorBase pRoot, 
                   const Mover& pMover,
                   InputTree& pTree);

  InputTree& insert(TreeIteratorBase pRoot,
                    const Mover& pMover,
                    const std::string& pNamespec,
                    const sys::fs::Path& pPath,
                    unsigned int pType = Input::Unknown);

  InputTree& insert(TreeIteratorBase pRoot,
                    const Mover& pMover,
                    const Input& pInput);

  InputTree& enterGroup(TreeIteratorBase pRoot,
                        const Mover& pMover);

  // -----  observers  ----- //
  unsigned int numOfInputs() const
  { return m_FileFactory.size(); }

  bool hasInput() const
  { return !m_FileFactory.empty(); }

private:
  InputFactory& m_FileFactory;

};

bool isGroup(const InputTree::iterator& pos);
bool isGroup(const InputTree::const_iterator& pos);
bool isGroup(const InputTree::dfs_iterator& pos);
bool isGroup(const InputTree::const_dfs_iterator& pos);
bool isGroup(const InputTree::bfs_iterator& pos);
bool isGroup(const InputTree::const_bfs_iterator& pos);

} // namespace of mcld

//===----------------------------------------------------------------------===//
// template member functions
template<size_t DIRECT>
mcld::InputTree&
mcld::InputTree::insert(mcld::TreeIteratorBase pRoot,
                        const std::string& pNamespec,
                        const mcld::sys::fs::Path& pPath,
                        unsigned int pType)
{
  BinTreeTy::node_type* node = createNode();
  node->data = m_FileFactory.produce(pNamespec, pPath, pType); 
  if (pRoot.isRoot())
    proxy::hook<TreeIteratorBase::Leftward>(pRoot.m_pNode,
        const_cast<const node_type*>(node));
  else
    proxy::hook<DIRECT>(pRoot.m_pNode,
        const_cast<const node_type*>(node));
  return *this;
}

template<size_t DIRECT>
mcld::InputTree&
mcld::InputTree::enterGroup(mcld::TreeIteratorBase pRoot)
{
  BinTreeTy::node_type* node = createNode(); 
  if (pRoot.isRoot())
    proxy::hook<TreeIteratorBase::Leftward>(pRoot.m_pNode,
        const_cast<const node_type*>(node));
  else
    proxy::hook<DIRECT>(pRoot.m_pNode,
        const_cast<const node_type*>(node));
  return *this;
}

template<size_t DIRECT>
mcld::InputTree& mcld::InputTree::insert(mcld::TreeIteratorBase pRoot,
	                                 const mcld::Input& pInput)
{
  BinTreeTy::node_type* node = createNode();
  node->data = const_cast<mcld::Input*>(&pInput);
  if (pRoot.isRoot())
    proxy::hook<TreeIteratorBase::Leftward>(pRoot.m_pNode,
                                         const_cast<const node_type*>(node));
  else
    proxy::hook<DIRECT>(pRoot.m_pNode,
                        const_cast<const node_type*>(node));
  return *this;
}

#endif

