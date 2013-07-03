//===- ListDigraph.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_GRAPHLITE_LIST_DIGRAPH_BASE_H
#define MCLD_GRAPHLITE_LIST_DIGRAPH_BASE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>

namespace mcld {
namespace graph {

/** \class ListDigraph
 *  \brief ListDigraph provides an linked-list inplementation of a graph.
 */
class ListDigraph
{
public:
  struct Node;
  struct Arc;

  struct Node {
  public:
    Node();

  public:
    Node *prev, *next;
    Arc *first_in, *first_out;
  };

  struct Arc {
  public:
    Arc();

  public:
    Node *target, *source;
    Arc *prev_in, *next_in;
    Arc *prev_out, *next_out;
  };

public:
  ListDigraph();

  Node* addNode();

  Arc* addArc(Node& pU, Node& pV);

  void erase(Node& pNode);

  void erase(Arc& pArc);

  void clear();

  void getHead(Node*& pNode) const { pNode = m_pNodeHead; }

private:
  typedef GCFactory<Node, 0> NodeList;
  typedef GCFactory<Arc, 0> ArcList;

private:
  Node* m_pNodeHead;
  Node* m_pFreeNodeHead;
  Arc*  m_pFreeArcHead;

  NodeList m_NodeList;
  ArcList m_ArcList;
};

} // namespace of graph
} // namespace of mcld

#endif

