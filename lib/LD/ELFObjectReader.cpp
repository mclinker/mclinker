//===- ELFObjectReader.cpp ------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ELFObjectReader.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLinker.h"
#include "mcld/Target/TargetLDBackend.h"

using namespace llvm;
using namespace mcld;

//==========================
// ELFObjectReader
ELFObjectReader::ELFObjectReader(TargetLDBackend& pBackend, MCLinker& pLinker)
  : ObjectReader(pBackend),
    ELFReader(),
    m_Linker(pLinker)
{
}

ELFObjectReader::~ELFObjectReader()
{
}

bool ELFObjectReader::isMyFormat(Input &pFile) const
{
  // TODO
  return false;
}

LDReader::Endian ELFObjectReader::endian(Input &pFile) const
{
  if (isLittleEndian(pFile))
    return LDReader::LittleEndian;
  return LDReader::BigEndian;
}

llvm::error_code ELFObjectReader::readObject(Input& pFile)
{
  // TODO
  return llvm::error_code();
}

bool ELFObjectReader::readSections(Input& pFile)
{
  // TODO
  return true;
}

