//===- Digraph.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/ADT/GraphLite/Digraph.h>

using namespace mcld::graph;

//===----------------------------------------------------------------------===//
// Digraph::Arc
//===----------------------------------------------------------------------===//
Digraph::Arc::Arc()
{
}

bool Digraph::Arc::operator==(const Digraph::Node& pOther) const
{
  return true;
}

bool Digraph::Arc::operator!=(const Digraph::Node& pOther) const
{
  return true;
}

Digraph::Node Digraph::Arc::source() const
{
  return Node();
}

Digraph::Node Digraph::Arc::target() const
{
  return Node();
}

Digraph::Arc::Arc(Digraph& pParent)
{
}


//===----------------------------------------------------------------------===//
// Digraph
//===----------------------------------------------------------------------===//
Digraph::Digraph()
{
}


Digraph::Node Digraph::addNode()
{
  return Node();
}


Digraph::Arc
Digraph::addArc(const Digraph::Node& pSource, const Digraph::Node& pTarget)
{
  return Arc();
}


void Digraph::erase(const Digraph::Node& pNode)
{
}


void Digraph::erase(const Digraph::Arc& pArc)
{
}


void Digraph::clear()
{
}

unsigned int Digraph::numOfNodes() const
{
  return 0;
}

unsigned int Digraph::numOfArcs() const
{
  return 0;
}
