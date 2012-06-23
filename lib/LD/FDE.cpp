//===- FDE.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/FDE.h>

using namespace mcld;

//==========================
// FDE

FDE::FDE(MemoryRegion& pRegion, const CIE& pCIE, Offset pPCBeginOffset)
  : MCRegionFragment(pRegion), m_CIE(pCIE), m_PCBeginOffset(pPCBeginOffset) {
}

FDE::~FDE()
{
}

