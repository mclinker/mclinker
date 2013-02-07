//===- FGEdge.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_FGEDGE_H
#define MCLD_FGEDGE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Fragment/FGNode.h>

namespace mcld
{

/** \class FGEdge
 *  \brief FGEdge presents the edge of FragmentGraph
 */
class FGEdge
{
public:
  FGEdge(FGNode& pFrom, FGNode& pTo, size_t pWeight)
   : m_From(&pFrom), m_To(&pTo), m_Weight(pWeight)
  {}

  ~FGEdge() {}

  /// ----- observers ----- ///
  FGNode* getFrom()
  { return m_From; }

  const FGNode* getFrom() const
  { return m_From; }

  FGNode* getTo()
  { return m_To; }

  const FGNode* getTo() const
  { return m_To; }

  size_t weight() const
  { return m_Weight; }

private:
  FGNode* m_From;
  FGNode* m_To;
  size_t m_Weight;
};

} // namespace of mcld

#endif

