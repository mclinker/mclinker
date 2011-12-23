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

ARMPLT::ARMPLT(llvm::MCSectionData* pSectionData, ARMGOT &pGOT)
  : PLT(pSectionData), m_GOT(pGOT)
{
}

ARMPLT::~ARMPLT()
{
}

void ARMPLT::reserveEntry(int pNum)
{
  ARMPLT1* plt1_entry = new ARMPLT1();
  m_pSectionData->getFragmentList().push_back(plt1_entry);

  GOTEntry* got_entry= new GOTEntry(0);
  m_GOT.GOTPLTEntries->getEntryList().push_back(got_entry);
}

PLTEntry* ARMPLT::getEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   // TODO
   return 0;
}

