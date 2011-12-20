//===- ARMPLT.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMGOT.h"
#include "ARMPLT.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMPLT

ARMPLT::ARMPLT(const LDSection& pSection, GOT& pGOTPLT)
  : PLT(pSection, pGOTPLT)
{
}

ARMPLT::~ARMPLT()
{
}

PLTEntry* ARMPLT::getOrCreateGOTPLT(ResolveInfo& pInfo) {
  ARMPLT1 *&plt1_entry = m_SymbolIndexMap[&pInfo];

  if (plt1_entry) {
    plt1_entry = new ARMPLT1();
    getFragmentList().push_back(plt1_entry);

    GOTEntry* got_entry= new GOTEntry(0, &m_GOTPLT);
    m_GOTPLT.getFragmentList().push_back(got_entry);
  }

  return plt1_entry;
}
