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

using namespace mcld;

//==========================
// X86GOT
X86GOT::X86GOT(llvm::MCSectionData* pSectionData)
  : GOT(pSectionData, 4)
{
}

X86GOT::~X86GOT()
{
}

void X86GOT::reserveEntry(int pNum)
{
}

X86GOT::Entry* X86GOT::getEntry(const ResolveInfo& pSymbol, bool& pExist)
{
  // TODO
  return 0;
}
