//===- MCELFArchiveReader.cpp ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCELFArchiveReader.h"

using namespace mcld;

//==========================
// MCELFArchiveReader
MCELFArchiveReader::MCELFArchiveReader(const MCELFArchiveTargetReader *pTargetReader)
  : m_pTargetReader(pTargetReader) {
}

MCELFArchiveReader::~MCELFArchiveReader()
{
}
 
