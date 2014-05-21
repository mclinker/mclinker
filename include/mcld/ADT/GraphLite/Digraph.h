//===- Digraph.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ADT_GRAPHLITE_DIGRAPH_H
#define MCLD_ADT_GRAPHLITE_DIGRAPH_H
#include <mcld/ADT/Uncopyable.h>
#include <mcld/ADT/GraphLite/GraphBasicTypes.h>
#include <stdint.h>

namespace mcld {
namespace graph {

/** \class Digraph
 *  \brief Digraph provides the common interface of all directed graphs.
 */
class Digraph : private Uncopyable
{
public:
  typedef DirectedTag direction_tag;

  class Node
  {
    friend class Digraph;
  public:
    Node() {}

    bool operator==(const Node& pOther) const { return m_ID == pOther.m_ID; }
    bool operator!=(const Node& pOther) const { return m_ID != pOther.m_ID; }

  protected:
    intptr_t m_ID;
  };

  class Arc
  {
    friend class Digraph;
  public:
    Arc();

    bool operator==(const Node& pOther) const;
    bool operator!=(const Node& pOther) const;

    Node source() const;
    Node target() const;

  protected:
    Arc(Digraph& pParent);

  protected:
    intptr_t m_ID;
    Digraph* m_Parent;
  };

public:
  Digraph();

  Node addNode();

  Arc addArc(const Node& pSource, const Node& pTarget);

  void erase(const Node& pNode);

  void erase(const Arc& pArc);

  void clear();

  unsigned int numOfNodes() const;

  unsigned int numOfArcs() const;

};

} // namespace of graph
} // namespace of mcld

#endif

