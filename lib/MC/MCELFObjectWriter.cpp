//===- MCELFObjectWriter.cpp ----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCELFObjectWriter.h"

using namespace mcld;

//==========================
// MCELFObjectWriter
MCELFObjectWriter::MCELFObjectWriter(const MCELFObjectTargetWriter *pTargetWriter)
  : m_pTargetWriter(pTargetWriter) {
}

MCELFObjectWriter::~MCELFObjectWriter()
{
}


