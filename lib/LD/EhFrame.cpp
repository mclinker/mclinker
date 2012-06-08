//===- EhFrame.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrame.h>

using namespace mcld;

//==========================
// EhFrame
EhFrame::EhFrame()
 : m_CIEFactory(32), m_FDEFactory(32) {
}

EhFrame::~EhFrame()
{
}

bool EhFrame::readEhFrame(MCLinker* pLinker,
                          const LDSection& pSection,
                          MemoryArea& pArea)
{
  return true;
}


bool EhFrame::readCIE(const MCRegionFragment& pFrag, bool pIsTargetLittleEndian)
{
  return true;
}

bool EhFrame::readFDE(const MCRegionFragment& pFrag, bool pIsTargetLittleEndian)
{
  return true;
}

uint32_t EhFrame::readVal(ConstAddress pAddr, bool pIsTargetLittleEndian)
{
  return 0;
}
