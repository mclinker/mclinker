//===- impl.cpp -----------------------------------------------------------===//
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
MipsGOT::MipsGOT()
  : GOT(new LDSection(LDFileFormat::GOT, ".got")) { // Is the name correct in Mips?
}

MipsGOT::~MipsGOT()
{
}

