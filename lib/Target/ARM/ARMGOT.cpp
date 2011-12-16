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

//==========================
// ARMGOT
ARMGOT::ARMGOT()
  : GOT(new LDSection(LDFileFormat::GOT, ".got")) {
}

ARMGOT::~ARMGOT()
{
}

