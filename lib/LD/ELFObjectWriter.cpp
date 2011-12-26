//===- ELFObjectWriter.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ELFObjectWriter.h"

using namespace mcld;

//==========================
// ELFObjectWriter
ELFObjectWriter::ELFObjectWriter(GNULDBackend& pBackend, MCLinker& pLinker)
  : ObjectWriter(pBackend), ELFWriter(pBackend), m_Linker(pLinker) {
}

ELFObjectWriter::~ELFObjectWriter()
{
}


