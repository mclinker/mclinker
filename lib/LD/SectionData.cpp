//===- SectionData.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/SectionData.h>
#include <mcld/LD/LDSection.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// SectionData
//===----------------------------------------------------------------------===//
SectionData::SectionData(const LDSection &pSection)
  : m_pSection(&pSection), m_Alignment(1) {
}
