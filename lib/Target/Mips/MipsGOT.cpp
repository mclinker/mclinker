//===- MipsGOT.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "MipsGOT.h"

using namespace mcld;

//==========================
// MipsGOT
MipsGOT::MipsGOT(LDSection& pSection, llvm::MCSectionData& pSectionData)
  : GOT(pSection, pSectionData, 4 /* Mips uses 32-bit GOT entry */) {
}

void MipsGOT::reserveEntry(int pNum)
{
}

GOTEntry* MipsGOT::getEntry(const ResolveInfo& pSymbol, bool& pExist)
{
  return 0;
}

MipsGOT::iterator MipsGOT::begin()
{
  return m_SectionData.getFragmentList().begin();
}

MipsGOT::iterator MipsGOT::end()
{
  return m_SectionData.getFragmentList().begin();
}

MipsGOT::const_iterator MipsGOT::begin() const
{
  return m_SectionData.getFragmentList().end();
}

MipsGOT::const_iterator MipsGOT::end() const
{
  return m_SectionData.getFragmentList().end();
}
