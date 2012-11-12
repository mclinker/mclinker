//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86GOT.h"

#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/SectionData.h>

namespace {
  const size_t X86GOTEntrySize = 4;
}

using namespace mcld;

//===----------------------------------------------------------------------===//
// X86GOT
//===----------------------------------------------------------------------===//
X86GOT::X86GOT(LDSection& pSection)
             : GOT(pSection, X86GOTEntrySize)
{
}

X86GOT::~X86GOT()
{
}

