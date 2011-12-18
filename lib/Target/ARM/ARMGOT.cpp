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

ARMGOT::Entry* ARMGOT::createEntry(uint64_t pData)
{
  return new Entry(pData, this);
}

