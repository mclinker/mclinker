//===- SectLinkerOption.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/CodeGen/SectLinkerOption.h"
#include "mcld/Support/DerivedPositionDependentOptions.h"
#include "mcld/Support/RealPath.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// SectLinkerOption
SectLinkerOption::SectLinkerOption(MCLDInfo &pLDInfo)
  : m_pLDInfo(&pLDInfo) { }

SectLinkerOption::~SectLinkerOption() {
  for (PositionDependentOptions::iterator pdoption = m_PosDepOptions.begin(),
       pdoptionEnd = m_PosDepOptions.end(); pdoption != pdoptionEnd; ++pdoption)
    delete *pdoption;
}
