//===- RelocationData.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/RelocationData.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// RelocationData
//===----------------------------------------------------------------------===//
RelocationData::RelocationData()
  : m_pSection(NULL), m_Alignment(0) {
}

RelocationData::RelocationData(const LDSection &pSection)
  : m_pSection(&pSection), m_Alignment(1) {
}

RelocationData* RelocationData::Create(const LDSection& pSection)
{
  return new RelocationData(pSection);
}

RelocationData* RelocationData::Create()
{
  return new RelocationData();
}

void RelocationData::Destroy(RelocationData*& pSection)
{
  pSection->~RelocationData();
  pSection = NULL;
}

