//===- BranchIslandFactory.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_BRANCHISLANDFACTORY_H
#define MCLD_LD_BRANCHISLANDFACTORY_H

#include <llvm/Support/DataTypes.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/LD/BranchIsland.h>

namespace mcld
{

class Fragment;
class Module;

/** \class BranchIslandFactory
 *  \brief
 *
 */
class BranchIslandFactory : public GCFactory<BranchIsland, 0>
{
public:
  /// ctor
  /// @param pMaxFwdBranchRange - the max forward branch range of the target
  /// @param pMaxBwdBranchRange - the max backward branch range of the target
  /// @param pMaxIslandSize - a predifned value (64KB here) to decide the max
  ///                         size of the island
  BranchIslandFactory(int64_t pMaxFwdBranchRange,
                      int64_t pMaxBwdBranchRange,
                      size_t pMaxIslandSize = 65536U);

  ~BranchIslandFactory();

  /// group - group fragments and create islands when needed
  /// @param pSectionData - the SectionData holds fragments need to be grouped
  void group(Module& pModule);

  /// produce - produce a island for the given fragment
  /// @param pFragment - the fragment needs a branch island
  BranchIsland* produce(Fragment& pFragment);

  /// getIsland - find fwd and bwd islands for the fragment
  /// @param pFragment - the fragment needs a branch island
  /// @return - return the pair of <fwd island, bwd island>
  std::pair<BranchIsland*, BranchIsland*> getIslands(const Fragment& pFragment);

private:
  int64_t m_MaxFwdBranchRange;
  int64_t m_MaxBwdBranchRange;
  size_t m_MaxIslandSize;
};

} // namespace of mcld

#endif

