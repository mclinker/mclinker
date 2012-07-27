//===- RegionFragment.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LD_REGION_FRAGMENT_H
#define MCLD_LD_REGION_FRAGMENT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/Fragment.h>
#include <mcld/Support/MemoryRegion.h>

namespace mcld
{

/** \class RegionFragment
 *  \brief RegionFragment is a kind of Fragment containing mcld::MemoryRegion
 */
class RegionFragment : public Fragment
{
public:
  RegionFragment(MemoryRegion& pRegion, SectionData* pSD = NULL);

  ~RegionFragment();

  MemoryRegion& getRegion()
  { return m_Region; }

  const MemoryRegion& getRegion() const
  { return m_Region; }

  static bool classof(const Fragment *F)
  { return F->getKind() == Fragment::Region; }

  static bool classof(const RegionFragment *)
  { return true; }

private:
  MemoryRegion& m_Region;
};

} // namespace of mcld

#endif

