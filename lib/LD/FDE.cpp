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

FDE::FDE(const MCRegionFragment& pFrag, const CIE& pCIE, Offset pPCBeginOffset)
  : m_Fragment(pFrag), m_CIE(pCIE), m_PCBeginOffset(pPCBeginOffset) {
}
FDE::~FDE()
{
}

const CIE& FDE::getCIE() const
{
  return m_CIE;
}

uint64_t FDE::PCBegin() const
{
  return 0;
}

