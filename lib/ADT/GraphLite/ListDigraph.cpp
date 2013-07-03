//===- ListDigraph.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/ADT/GraphLite/ListDigraph.h>

using namespace mcld::graph;

//===----------------------------------------------------------------------===//
// ListDigraph::Node
//===----------------------------------------------------------------------===//
ListDigraph::Node::Node()
  : prev(NULL), next(NULL), first_in(NULL), first_out(NULL) {
}

//===----------------------------------------------------------------------===//
// ListDigraph::Arc
//===----------------------------------------------------------------------===//
ListDigraph::Arc::Arc()
  : target(NULL), source(NULL),
    prev_in(NULL), next_in(NULL), prev_out(NULL), next_out(NULL) {
}

//===----------------------------------------------------------------------===//
// ListDigraph
//===----------------------------------------------------------------------===//
ListDigraph::ListDigraph()
  : m_pNodeHead(NULL), m_pFreeNodeHead(NULL), m_pFreeArcHead(NULL),
    m_NodeList(32), m_ArcList(32) {
}


ListDigraph::Node* ListDigraph::addNode()
{
  // 1. find an available free node
  Node* result = NULL;
  if (NULL == m_pFreeNodeHead) {
    result = m_NodeList.allocate();
    new (result) Node();
  }
  else {
    result = m_pFreeNodeHead;
    m_pFreeNodeHead = m_pFreeNodeHead->next;
  }

  // 2. set up linkages
  result->prev = NULL;
  result->next = m_pNodeHead;

  // 3. reset head node
  if (NULL != m_pNodeHead) {
    m_pNodeHead->prev = result;
  }
  m_pNodeHead = result;

  return result;
}


ListDigraph::Arc* ListDigraph::addArc(Node& pU, Node& pV)
{
  // 1. find an available free arc
  Arc* result = NULL;
  if (NULL == m_pFreeArcHead) {
    result = m_ArcList.allocate();
    new (result) Arc();
  }
  else {
    result = m_pFreeArcHead;
    m_pFreeArcHead = m_pFreeArcHead->next_in;
  }

  // 2. set up arc
  result->source = &pU;
  result->target = &pV;

  // 3. set up fan-out linked list
  result->next_out = pU.first_out;
  if (NULL != pU.first_out) {
    pU.first_out->prev_out = result;
  }
  pU.first_out = result;

  // 4. set up fan-in linked list
  result->next_in = pV.first_in;
  if (NULL != pV.first_in) {
    pV.first_in->prev_in = result;
  }
  pV.first_in = result;

  return result;
}

void ListDigraph::erase(ListDigraph::Node& pNode)
{
  // 1. connect previous node and next node.
  if (NULL != pNode.next) {
    pNode.next->prev = pNode.prev;
  }

  if (NULL != pNode.prev) {
    pNode.prev->next = pNode.next;
  }
  else { // pNode.prev is NULL => pNode is the head
    m_pNodeHead = pNode.next;
  }

  // 2. remove all fan-in arcs
  Arc* fan_in = pNode.first_in;
  while(NULL != fan_in) {
    Arc* next_in = fan_in->next_in;
    erase(*fan_in);
    fan_in = next_in;
  }

  // 3. remove all fan-out arcs
  Arc* fan_out = pNode.first_out;
  while(NULL != fan_out) {
    Arc* next_out = fan_out->next_out;
    erase(*fan_out);
    fan_out = next_out;
  }

  // 4. put pNode in the free node list
  pNode.next = m_pFreeNodeHead;
  pNode.prev = NULL;
  if (NULL != m_pFreeNodeHead)
    m_pFreeNodeHead->prev = &pNode;
  m_pFreeNodeHead = &pNode;
}


void ListDigraph::erase(ListDigraph::Arc& pArc)
{
  // 1. remove from the fan-out list
  if (NULL != pArc.prev_out) {
    pArc.prev_out->next_out = pArc.next_out;
  }
  else { // pArc.prev_out is NULL => pArc is the first_out of the source
    pArc.source->first_out = pArc.next_out;
  }

  if (NULL != pArc.next_out) {
    pArc.next_out->prev_out = pArc.prev_out;
  }

  // 2. remove from the fan-in list
  if (NULL != pArc.prev_in) {
    pArc.prev_in->next_in = pArc.next_in;
  }
  else {
    pArc.target->first_in = pArc.next_in;
  }

  if (NULL != pArc.next_in) {
    pArc.next_in->prev_in = pArc.prev_in;
  }

  // 3. put pArc in the free arc list
  // Use fan-in links to chain the free list
  pArc.next_in = m_pFreeArcHead;
  m_pFreeArcHead = &pArc;
}


void ListDigraph::clear()
{
  m_pNodeHead = NULL;
  m_pFreeNodeHead = NULL;
  m_pFreeArcHead = NULL;
  m_NodeList.clear();
  m_ArcList.clear();
}

