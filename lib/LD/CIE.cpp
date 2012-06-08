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

//==========================
// CIE

CIE::CIE(const MCRegionFragment& pFrag, uint8_t pFDEEncode)
  : m_Fragment(pFrag), m_FDEEncoding(pFDEEncode) {
}

CIE::~CIE()
{
}
