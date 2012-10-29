//===- RelocData.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/RelocData.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// RelocData
//===----------------------------------------------------------------------===//
RelocData::RelocData()
  : m_pSection(NULL) {
}

RelocData::RelocData(const LDSection &pSection)
  : m_pSection(&pSection) {
}

RelocData* RelocData::Create(const LDSection& pSection)
{
  return new RelocData(pSection);
}

RelocData* RelocData::Create()
{
  return new RelocData();
}

void RelocData::Destroy(RelocData*& pSection)
{
  delete pSection;
  pSection = NULL;
}

