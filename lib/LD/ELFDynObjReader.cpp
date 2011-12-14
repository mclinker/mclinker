//===- ELFDynObjReader.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFDynObjReader.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDInput.h>

using namespace mcld;

//==========================
// ELFDynObjReader
ELFDynObjReader::ELFDynObjReader(TargetLDBackend& pBackend, MCLinker& pLinker)
  : DynObjReader(pBackend), m_Linker(pLinker) {
}

ELFDynObjReader::~ELFDynObjReader()
{
}

LDReader::Endian ELFDynObjReader::endian(Input& pFile) const
{
  // TODO
  return LDReader::LittleEndian;
}

bool ELFDynObjReader::isMyFormat(Input &pFile) const
{
  // TODO
  return false;
}

llvm::error_code ELFDynObjReader::readDSO(Input& pFile)
{
  // TODO
  return llvm::error_code();
}

bool ELFDynObjReader::readSymbols(Input& pFile)
{
  return ELFReader::readDynSymbols(pFile);
}

