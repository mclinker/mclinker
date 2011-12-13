//===- ObjectWriter.cpp ---------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ObjectWriter.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/Target/TargetLDBackend.h"

using namespace mcld;

//==========================
// ObjectWriter
ObjectWriter::ObjectWriter(TargetLDBackend& pBackend)
  : m_LDBackend(pBackend) {
}

ObjectWriter::~ObjectWriter()
{
}

