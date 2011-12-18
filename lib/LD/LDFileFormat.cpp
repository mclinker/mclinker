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
#include <mcld/LD/LDContext.h>

using namespace mcld;

//==========================
// LDFileInfo
LDFileFormat::LDFileFormat()
{
}

LDFileFormat::~LDFileFormat()
{
}

void LDFileFormat::initStdSections(MCLinker& pLinker, LDContext& pContext)
{
  initObjectFormat(pLinker, pContext);
  initObjectType(pLinker, pContext);
}

