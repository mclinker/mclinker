//===- ELFDynObjWriter.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ELFDynObjWriter.h"
#include "mcld/Target/TargetLDBackend.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLDOutput.h"
#include "mcld/MC/MCLDInfo.h"
#include "mcld/MC/MCLinker.h"
#include "mcld/Support/ScopedWriter.h"

using namespace mcld;

//==========================
// ELFDynObjWriter
ELFDynObjWriter::ELFDynObjWriter(TargetLDBackend& pBackend, MCLinker& pLinker)
  : DynObjWriter(pBackend), ELFWriter(), m_Linker(pLinker) {
}

ELFDynObjWriter::~ELFDynObjWriter()
{
}

bool ELFDynObjWriter::WriteObject()
{
  if (!WriteHeader())
    return false;

  return true;
}

bool ELFDynObjWriter::WriteHeader()
{
  return true;
}
