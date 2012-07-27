//===- CIE.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/CIE.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// CIE
//===----------------------------------------------------------------------===//
CIE::CIE(MemoryRegion& pRegion, uint8_t pFDEEncode)
  : RegionFragment(pRegion), m_FDEEncoding(pFDEEncode) {
}

CIE::~CIE()
{
}
