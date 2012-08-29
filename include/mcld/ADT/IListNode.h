//===- IListNode.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ADT_ILIST_NODE_H
#define MCLD_ADT_ILIST_NODE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/ilist_node.h>

namespace mcld {

template<typename NodeTy>
struct ilist_traits;

template<typename NodeTy>
struct ilist_nextprev_traits;

template<typename NodeTy>
class IListNode : private llvm::ilist_half_node<NodeTy>
{
  friend struct llvm::ilist_nextprev_traits<NodeTy>;
  friend struct mcld::ilist_nextprev_traits<NodeTy>;
  friend struct llvm::ilist_traits<NodeTy>;
  friend struct mcld::ilist_traits<NodeTy>;

protected:
  IListNode() : m_pNext(NULL) { }

  ~IListNode() {
    // remove myself from the list.
    NodeTy* next = getNext();
    NodeTy* prev = this->getPrev();

    if (NULL != prev)
      prev->setNext(next);

    if (NULL != next)
      next->setPrev(prev);
  }

public:
  NodeTy* getPrevNode() {
    NodeTy* prev = this->getPrev();

    // Check for sentinel.
    if (NULL == prev->getNext())
      return NULL;

    return prev;
  }

  const NodeTy* getPrevNode() const {
    const NodeTy* prev = this->getPrev();

    // Check for sentinel.
    if (NULL == prev->getNext())
      return NULL;

    return prev;
  }

  NodeTy* getNextNode() {
    NodeTy* next = getNext();

    // Check for sentinel.
    if (NULL == next->getNext())
      return NULL;

    return next;
  }

  const NodeTy* getNextNode() const {
    const NodeTy* next = getNext();

    // Check for sentinel.
    if (NULL == next->getNext())
      return NULL;

    return next;
  }

private:
  const NodeTy* getNext() const { return m_pNext; }
  NodeTy*       getNext()       { return m_pNext; }

  void setNext(NodeTy* pNext) { m_pNext = pNext; }

private:
  NodeTy* m_pNext;
};

} // namespace of mcld

#endif

