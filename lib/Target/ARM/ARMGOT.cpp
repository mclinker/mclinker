//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMGOT.h"
#include "mcld/LD/LDFileFormat.h"
#include <llvm/Support/ErrorHandling.h>
#include <new>

namespace mcld {

//===----------------------------------------------------------------------===//
// ARMGOT
ARMGOT::ARMGOT(llvm::MCSectionData& pSectionData)
             : m_pGOT0Entries(new (std::nothrow) ARMGOTEntries),
               m_pGOTPLTEntries(new (std::nothrow) ARMGOTEntries),
               m_pGeneralGOTEntries(new (std::nothrow) ARMGOTEntries),
               m_GeneralGOTIterator(),
               GOT(pSectionData, 4 /*ARM uses 32-bit GOT entry */) {

  pSectionData.getFragmentList().push_back(m_pGOT0Entries);
  pSectionData.getFragmentList().push_back(m_pGOTPLTEntries);
  pSectionData.getFragmentList().push_back(m_pGeneralGOTEntries);

  // Create GOT0 entries.
  GOTEntry* Entry1 = new (std::nothrow) GOTEntry(0);
  m_pGOT0Entries->getEntryList().push_back(Entry1);

  GOTEntry* Entry2 = new (std::nothrow) GOTEntry(0);
  m_pGOT0Entries->getEntryList().push_back(Entry2);

  GOTEntry* Entry3 = new (std::nothrow) GOTEntry(0);
  m_pGOT0Entries->getEntryList().push_back(Entry3);

}

ARMGOT::~ARMGOT()
{
}

void ARMGOT::reserveEntry(const int pNum)
{
  if (!m_pGeneralGOTEntries)
    llvm::report_fatal_error("m_pGeneralGOTEntries is NULL! "
                             "Allocating new memory in constructor failed" );

  GOTEntry* Entry = 0;

  for (int i = 0; i < pNum; i++) {
    Entry = new (std::nothrow) GOTEntry(0);

    if (m_pGeneralGOTEntries->size() == 1)
      m_GeneralGOTIterator = m_pGeneralGOTEntries->begin();

    if (!Entry)
      llvm::report_fatal_error("Allocating new memory for GOTEntry failed");

    m_pGeneralGOTEntries->getEntryList().push_back(Entry);
  }
}


GOTEntry* ARMGOT::getEntry(const ResolveInfo* pInfo, bool& pExist)
{
  assert((!(pInfo)) && "ResolvedInfo is NULL!");

  GOTEntry *&GeneralGOTEntry = m_SymbolIndexMap[pInfo];
  pExist = 1;

  if (!GeneralGOTEntry) {
    pExist = 0;

    assert(m_GeneralGOTIterator != m_pGeneralGOTEntries->getEntryList().end()
           && "The number of GOT Entries and ResolveInfo doesn't match!");

    GeneralGOTEntry = llvm::cast<GOTEntry>(&(*m_GeneralGOTIterator));
    m_GeneralGOTIterator++;
  }

  return GeneralGOTEntry;
}

void ARMGOT::applyGOT0(const uint64_t pAddress)
{
  if (!m_pGOT0Entries)
    llvm::report_fatal_error("m_pGOT0Entries is NULL! "
                             "Allocating new memory in constructor failed" );

  llvm::cast<GOTEntry>
    (*(m_pGeneralGOTEntries->getEntryList().begin())).setContent(pAddress);
}

void ARMGOT::applyGOTPLT(const uint64_t pAddress)
{
  if (!m_pGOTPLTEntries)
    llvm::report_fatal_error("m_pGOTPLTEntries is NULL! "
                             "Allocating new memory in constructor failed" );

  for (GOTIteratorType it = m_pGOTPLTEntries->getEntryList().begin(),
       ie = m_pGOTPLTEntries->getEntryList().end(); it != ie; it++)
    llvm::cast<GOTEntry>(*(it)).setContent(pAddress);
}

} //end mcld namespace
