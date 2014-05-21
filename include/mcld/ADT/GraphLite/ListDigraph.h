//===- ListDigraph.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ADT_GRAPHLITE_LISTDIGRAPH_H
#define MCLD_ADT_GRAPHLITE_LISTDIGRAPH_H
#include <mcld/Support/GCFactory.h>

namespace mcld {
namespace graph {

/** \class ListDigraph
 *  \brief ListDigraph provides an linked-list inplementation of a graph.
 *
 *  ListDigraph is designed to get well performance for most algorithms of
 *  graph theory.
 *
 *  Function        | Complexity | Best Complexity
 *  ----------------|------------|--------------------------
 *  Storage         | V + E      |
 *  Add node        | O(1)       |
 *  Add arc         | O(1)       |
 *  Remove node     | O(E)       | O(#(fan-in) + #(fan-out))
 *  Remove edge     | O(1)       |
 *  Query adjacency | O(E)       | O(#(fan-in) + #(fan-out))
 *
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

