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
               m_GOTPLTNum(0), m_NormalGOTNum(0), m_LastGOT0(),
               m_GOTPLTIterator(), m_NormalGOTIterator()
{
  GOTEntry* Entry = 0;

  // Create GOT0 entries.
  for (int i = 0; i < 3; i++) {
    Entry = new (std::nothrow) GOTEntry(0, ARMGOTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      llvm::report_fatal_error("Allocating GOT0 entries failed!");

    m_Section.setSize(m_Section.size() + ARMGOTEntrySize);
  }

  // Skip GOT0 entries.
  iterator it = m_SectionData.begin();
  iterator ie = m_SectionData.end();

  for (int i = 1; i < ARMGOT0Num; ++i) {
    if (it == ie)
      llvm::report_fatal_error("Generation of GOT0 entries is incomplete!");

    ++it;
  }

  m_LastGOT0 = it;
  m_NormalGOTIterator = it;
  m_GOTPLTIterator = it;
}

ARMGOT::~ARMGOT()
{
}

void ARMGOT::reserveEntry(const int pNum)
{
  GOTEntry* Entry = 0;

  for (int i = 0; i < pNum; i++) {
    Entry = new (std::nothrow) GOTEntry(0, ARMGOTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      llvm::report_fatal_error("Allocating new memory for GOTEntry failed");

    m_Section.setSize(m_Section.size() + ARMGOTEntrySize);
    ++m_NormalGOTNum;
  }
}

void ARMGOT::reserveGOTPLTEntry()
{
    GOTEntry* got_entry = 0;

    got_entry= new GOTEntry(0, getEntrySize(),&(getSectionData()));

    if (!got_entry)
      llvm::report_fatal_error("Allocating new memory for GOT failed!");

    m_Section.setSize(m_Section.size() + getEntrySize());

    ++m_GOTPLTNum;
    ++m_NormalGOTIterator;
}

GOTEntry* ARMGOT::getEntry(const ResolveInfo& pInfo, bool& pExist)
{
  GOTEntry *&Entry = m_NormalGOTMap[&pInfo];
  pExist = 1;

  if (!Entry) {
    pExist = 0;

    ++m_NormalGOTIterator;
    assert(m_NormalGOTIterator != m_SectionData.getFragmentList().end()
           && "The number of GOT Entries and ResolveInfo doesn't match!");

    Entry = llvm::cast<GOTEntry>(&(*m_NormalGOTIterator));
  }

  return Entry;
}

void ARMGOT::applyGOT0(const uint64_t pAddress)
{
  llvm::cast<GOTEntry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

ARMGOT::iterator ARMGOT::begin()
{
  return m_SectionData.getFragmentList().begin();
}

ARMGOT::const_iterator ARMGOT::begin() const
{
  return m_SectionData.getFragmentList().begin();
}

ARMGOT::iterator ARMGOT::end()
{
  return m_SectionData.getFragmentList().end();
}

ARMGOT::const_iterator ARMGOT::end() const
{
  return m_SectionData.getFragmentList().end();
}

unsigned int ARMGOT::getGOTPLTNum()
{ return m_GOTPLTNum; }

const unsigned int ARMGOT::getGOTPLTNum() const
{ return m_GOTPLTNum; }

ARMGOT::iterator ARMGOT::getLastGOT0()
{ return m_LastGOT0; }

const ARMGOT::iterator ARMGOT::getLastGOT0() const
{ return m_LastGOT0; }

} //end mcld namespace
