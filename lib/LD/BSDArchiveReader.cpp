//===- BSDArchiveReader.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLDInputTree.h"
#include "mcld/LD/BSDArchiveReader.h"

using namespace mcld;

BSDArchiveReader::BSDArchiveReader()
{
}

BSDArchiveReader::~BSDArchiveReader()
{
}

InputTree *BSDArchiveReader::readArchive(Input &input)
{
  // TODO
  return NULL;
}

bool BSDArchiveReader::isMyFormat(Input& pInput) const
{
  // TODO
  return false;
}

