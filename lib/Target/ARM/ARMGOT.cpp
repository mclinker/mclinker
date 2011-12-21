//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMGOT.h"
#include <mcld/LD/LDFileFormat.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMGOT
ARMGOT::ARMGOT(const LDSection& pSection)
  : GOT(pSection, 4) // ARM uses 32-bit GOT entry
{
}

ARMGOT::~ARMGOT()
{
}

void ARMGOT::reserveEntry(int pNum)
{
}

ARMGOT::Entry* ARMGOT::getEntry(const ResolveInfo& pSymbol, bool& pExist)
{
  // TODO
  return 0;
}
