//===- ELFReader.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ELFReader.h"

using namespace mcld;

//==========================
// ELFReader
Input::Type ELFReader::fileType(mcld::Input &pFile) const
{
  return Input::Object; // TODO
}

bool ELFReader::isLittleEndian(mcld::Input &pFile) const
{
  return true; // TODO
}
  
bool ELFReader::is64Bit(mcld::Input &pFile) const
{
  return false; // TODO
}

bool ELFReader::readDynSymbols(mcld::Input &pFile) const
{
  return false; // TODO
}

