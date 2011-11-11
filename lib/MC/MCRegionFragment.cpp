//===- MCRegionFragment.cpp - MCRegionFragment implementation -------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/MC/MCRegionFragment.h"

using namespace mcld;
using namespace llvm;

//==========================
// MCRegionFragment
MCRegionFragment::MCRegionFragment(MemoryRegion& pRegion, MCSectionData* pSD)
  : MCFragment((llvm::MCFragment::FragmentType)FT_Region, pSD), m_Region(pRegion) {
}

MCRegionFragment::~MCRegionFragment()
{
}

