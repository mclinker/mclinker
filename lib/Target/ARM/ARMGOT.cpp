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

namespace {
  const uint64_t ARMGOTEntrySize = 4;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// ARMGOT
ARMGOT::ARMGOT(LDSection& pSection, llvm::MCSectionData& pSectionData)
             : GOT(pSection, pSectionData, ARMGOTEntrySize),
               m_GOTPLTNum(0), m_GeneralGOTNum(0), m_GeneralGOTIterator()
{
  GOTEntry* Entry = 0;

  // Create GOT0 entries.
  for (int i = 0; i < 3; i++) {
    Entry = new (std::nothrow) GOTEntry(0, ARMGOTEntrySize);

    if (!Entry)
      llvm::report_fatal_error("Allocating GOT0 entries failed!");

    m_SectionData.getFragmentList().push_back(Entry);
  }

  // Skip GOT0
  m_GeneralGOTIterator = m_SectionData.begin();
  ++m_GeneralGOTIterator;
  ++m_GeneralGOTIterator;
}

ARMGOT::~ARMGOT()
{
}

void ARMGOT::reserveEntry(const int pNum)
{
  GOTEntry* Entry = 0;

  for (int i = 0; i < pNum; i++) {
    Entry = new (std::nothrow) GOTEntry(0, ARMGOTEntrySize);

    if (!Entry)
      llvm::report_fatal_error("Allocating new memory for GOTEntry failed");

    m_SectionData.getFragmentList().push_back(Entry);
    ++m_GeneralGOTNum;
  }
}


GOTEntry* ARMGOT::getEntry(const ResolveInfo& pInfo, bool& pExist)
{
  GOTEntry *&Entry = m_SymbolIndexMap[&pInfo];
  pExist = 1;

  if (!Entry) {
    pExist = 0;

    ++m_GeneralGOTIterator;
    assert(m_GeneralGOTIterator != m_SectionData.getFragmentList().end()
           && "The number of GOT Entries and ResolveInfo doesn't match!");

    Entry = llvm::cast<GOTEntry>(&(*m_GeneralGOTIterator));
  }

  return Entry;
}

void ARMGOT::applyGOT0(const uint64_t pAddress)
{
  llvm::cast<GOTEntry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

void ARMGOT::applyGOTPLT(const uint64_t pAddress)
{
  iterator it = m_SectionData.getFragmentList().begin();

  // Skip GOT0
  ++it;
  ++it;
  ++it;

  for (int i = 0; i < m_GOTPLTNum; ++i)
    llvm::cast<GOTEntry>(*(it)).setContent(pAddress);
}

} //end mcld namespace
