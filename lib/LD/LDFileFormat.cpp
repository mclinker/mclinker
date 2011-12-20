//===- LDFileFormat.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDFileFormat.h>
#include <mcld/MC/MCLinker.h>

using namespace mcld;

//==========================
// LDFileInfo
LDFileFormat::LDFileFormat()
{
}

LDFileFormat::~LDFileFormat()
{
}

void LDFileFormat::initStdSections(MCLinker& pLinker)
{
  initObjectFormat(pLinker);
  initObjectType(pLinker);
}

