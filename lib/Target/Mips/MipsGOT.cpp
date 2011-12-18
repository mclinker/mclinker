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
MipsGOT::MipsGOT(const LDSection& pSection)
  : GOT(pSection, 4) { // Does Mips use 32-bit GOT entry?
}

MipsGOT::~MipsGOT()
{
}

MipsGOT::Entry* MipsGOT::createEntry(uint64_t pData)
{
  return new Entry(pData, this);
}

