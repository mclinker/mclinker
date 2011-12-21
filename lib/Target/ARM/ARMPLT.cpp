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

ARMPLT::ARMPLT(const LDSection& pSection)
  : PLT(pSection)
{
}

ARMPLT::~ARMPLT()
{
}

PLTEntry* ARMPLT::getOrCreateGOTPLT(const ResolveInfo& pInfo) {
  ARMPLT1 *&plt1_entry = m_SymbolIndexMap[&pInfo];

  if (plt1_entry) {
    plt1_entry = new ARMPLT1();
    getFragmentList().push_back(plt1_entry);
// FIXME: do we still need to create got entry at the same time with plt?
//        while we hope to let got and plt section maintain their own empty entry.
//    GOTEntry* got_entry= new GOTEntry(0, &m_GOTPLT);
//    m_GOTPLT.getFragmentList().push_back(got_entry);
  }

  return plt1_entry;
}

void ARMPLT::reserveEntry(int pNum)
{
}

PLTEntry* ARMPLT::getEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   // TODO
   return 0;
}
