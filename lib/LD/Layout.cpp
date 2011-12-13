//===- Layout.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/LD/Layout.h"
#include "mcld/LD/LDFileFormat.h"
#include "mcld/LD/MCLinker.h"

using namespace mcld;

Layout::Layout()
{
}

Layout::~Layout()
{
}

uint64_t Layout::layoutFragment(MCFragment& pFrag)
{
}

bool Layout::layout(MCLinker& pLinker, LDFileFormat& pFormat)
{
}

