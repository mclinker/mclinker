//===- ARMPLT.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMPLT.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMPLT

ARMPLT::ARMPLT(const LDSection& pSection, GOT& pGOTPLT)
  : PLT(pSection, pGOTPLT)
{
}

ARMPLT::~ARMPLT()
{
}

void ARMPLT::addEntry(LDSymbol& pSym)
{
}
