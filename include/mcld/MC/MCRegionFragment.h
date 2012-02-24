//===- MCRegionFragment.h - MCFragment containing MemoryRegion ------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_REGION_FRAGMENT_H
#define MCLD_REGION_FRAGMENT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Support/MemoryRegion.h>
#include <llvm/MC/MCAssembler.h>

namespace mcld
{

/** \class MCRegionFragment
 *  \brief MCRegionFragment is a kind of MCFragment containing 
 *  mcld::MemoryRegion
 */
class MCRegionFragment : public llvm::MCFragment
{
public:
  MCRegionFragment(MemoryRegion& pRegion, llvm::MCSectionData* pSD = 0);
  ~MCRegionFragment();

  MemoryRegion& getRegion()
  { return m_Region; }

  const MemoryRegion& getRegion() const
  { return m_Region; }

  static bool classof(const MCFragment *F)
  { return F->getKind() == llvm::MCFragment::FT_Region; }

  static bool classof(const MCRegionFragment *)
  { return true; }

private:
  MemoryRegion& m_Region;
  llvm::MCSectionData* m_pSectionData;

};

} // namespace of mcld

#endif

