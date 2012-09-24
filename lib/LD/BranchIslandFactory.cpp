//===- BranchIslandFactory.cpp --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/BranchIslandFactory.h>
#include <mcld/LD/Layout.h>
#include <mcld/Fragment/Fragment.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// BranchIslandFactory
//===----------------------------------------------------------------------===//

/// ctor
/// @param pMaxBranchRange - the max branch range of the target backend
/// @param pMaxIslandSize - a predifned value (1KB here) to decide the max
///                         size of the island
BranchIslandFactory::BranchIslandFactory(uint64_t pMaxBranchRange,
                                         uint64_t pMaxIslandSize)
 : GCFactory<BranchIsland, 0>(1u),
   m_MaxBranchRange(pMaxBranchRange),
   m_MaxIslandSize(pMaxIslandSize)
{
}

BranchIslandFactory::~BranchIslandFactory()
{
}

/// produce - produce a island for the given fragment
/// @param pFragment - the fragment needs a branch island
BranchIsland* BranchIslandFactory::produce(Fragment& pFragment)
{
  return NULL;
}

/// find - find a island for the given fragment
/// @param pFragment - the fragment needs a branch isladn
BranchIsland* BranchIslandFactory::find(const Fragment& pFragment)
{
  return NULL;
}

