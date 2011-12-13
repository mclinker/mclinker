//===- ELFDynObjWriter.cpp ------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ELFDynObjWriter.h"
#include "mcld/MC/MCLinker.h"
#include "mcld/Target/TargetLDBackend.h"
#include "mcld/MC/MCLDInput.h"

using namespace mcld;

//==========================
// ELFDynObjWriter
ELFDynObjWriter::ELFDynObjWriter(TargetLDBackend& pBackend, MCLinker& pLinker)
  : DynObjWriter(pBackend), ELFWriter(), m_Linker(pLinker) {
}

ELFDynObjWriter::~ELFDynObjWriter()
{
}

