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
MipsGOT::MipsGOT(const std::string pSectionName)
  : GOT(pSectionName)
{
}

MipsGOT::~MipsGOT()
{
}

