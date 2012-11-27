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
RelocData::RelocData(LDSection &pSection)
  : m_Section(pSection) {
}

RelocData* RelocData::Create(LDSection& pSection)
{
  return new RelocData(pSection);
}

void RelocData::Destroy(RelocData*& pSection)
{
  delete pSection;
  pSection = NULL;
}

RelocData& RelocData::append(Relocation& pRelocation)
{
  m_Relocations.push_back(&pRelocation);
  return *this;
}

