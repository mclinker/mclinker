//===- BranchIslandFactory.cpp --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/BranchIslandFactory.h>
#include <mcld/Fragment/Fragment.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Module.h>

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
 : GCFactory<BranchIsland, 0>(1u), // magic number
   m_MaxBranchRange(pMaxBranchRange - pMaxIslandSize),
   m_MaxIslandSize(pMaxIslandSize)
{
}

BranchIslandFactory::~BranchIslandFactory()
{
}

/// group - group fragments and create islands when needed
/// @param pSectionData - the SectionData holds fragments need to be grouped
void BranchIslandFactory::group(Module& pModule)
{
  /* Currently only support relaxing .text section! */
  LDSection* text = pModule.getSection(".text");
  if (text != NULL && text->hasSectionData()) {
    SectionData& sd = *text->getSectionData();
    uint64_t group_end = m_MaxBranchRange - m_MaxIslandSize;
    for (SectionData::iterator it = sd.begin(), ie = sd.end(); it != ie; ++it) {
      if ((*it).getOffset() + (*it).size() > group_end) {
        Fragment* frag = (*it).getPrevNode();
        while (frag != NULL && frag->getKind() == Fragment::Alignment) {
          frag = frag->getPrevNode();
        }
        if (frag != NULL) {
          produce(*frag);
          group_end = (*it).getOffset() + m_MaxBranchRange - m_MaxIslandSize;
        }
      }
    }
    if (find(sd.back()) == NULL)
      produce(sd.back());
  }
}

/// produce - produce a island for the given fragment
/// @param pFragment - the fragment needs a branch island
BranchIsland* BranchIslandFactory::produce(Fragment& pFragment)
{
  BranchIsland *island = allocate();
  new (island) BranchIsland(pFragment,       // entry fragment to the island
                            m_MaxIslandSize, // the max size of the island
                            size() - 1u);    // index in the island factory
  return island;
}

/// find - find a island for the given fragment
/// @param pFragment - the fragment needs a branch isladn
BranchIsland* BranchIslandFactory::find(const Fragment& pFragment)
{
  // Currently we always find the island in a forward direction.
  // TODO: If we can search backward, then we may reduce the number of stubs.
  for (iterator it = begin(), ie = end(); it != ie; ++it) {
    if ((pFragment.getOffset() < (*it).offset()) &&
        ((pFragment.getOffset() + m_MaxBranchRange) >= (*it).offset()))
      return &(*it);
  }
  return NULL;
}

