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

MipsGOT::~MipsGOT()
{
}

void MipsGOT::reserveEntry(int pNum)
{
}

MipsGOT::Entry* MipsGOT::getEntry(const ResolveInfo& pSymbol, bool& pExist)
{
  return 0;
}
