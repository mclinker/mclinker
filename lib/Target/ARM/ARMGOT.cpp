//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMGOT.h"

#include <new>

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDFileFormat.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>

namespace {
  const size_t ARMGOTEntrySize = 4;
} // end of anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMGOT
ARMGOT::ARMGOT(LDSection& pSection, SectionData& pSectionData)
             : GOT(pSection, pSectionData, ARMGOTEntrySize),
               m_NormalGOTIterator(), m_GOTPLTIterator(),
               m_GOTPLTBegin(), m_GOTPLTEnd()
{
  GOTEntry* Entry = 0;

  // Create GOT0 entries.
  for (int i = 0; i < 3; i++) {
    Entry = new (std::nothrow) GOTEntry(0, ARMGOTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + ARMGOTEntrySize);
  }

  // Skip GOT0 entries.
  iterator it = m_SectionData.begin();

  for (int i = 1; i < ARMGOT0Num; ++i) {
    assert((it != m_SectionData.end()) && "Generation of GOT0 entries is incomplete!");
    ++it;
  }

  m_NormalGOTIterator = it;
  m_GOTPLTIterator = it;

  m_GOTPLTBegin = it;
  m_GOTPLTEnd = it;
}

ARMGOT::~ARMGOT()
{
}

void ARMGOT::reserveEntry(size_t pNum)
{
  GOTEntry* Entry = 0;

  for (size_t i = 0; i < pNum; i++) {
    Entry = new (std::nothrow) GOTEntry(0, ARMGOTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + ARMGOTEntrySize);
  }
}

void ARMGOT::reserveGOTPLTEntry()
{
    GOTEntry* got_entry = 0;

    got_entry= new GOTEntry(0, getEntrySize(),&(getSectionData()));

    if (!got_entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + getEntrySize());

    ++m_GOTPLTEnd;
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

void ARMGOT::applyGOT0(uint64_t pAddress)
{
  llvm::cast<GOTEntry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

void ARMGOT::applyAllGOTPLT(uint64_t pPLTBase)
{
  iterator begin = getGOTPLTBegin();
  iterator end = getGOTPLTEnd();

  for (;begin != end ;++begin)
    llvm::cast<GOTEntry>(*begin).setContent(pPLTBase);
}

GOTEntry*& ARMGOT::lookupGOTPLTMap(const ResolveInfo& pSymbol)
{
  return m_GOTPLTMap[&pSymbol];
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

ARMGOT::iterator ARMGOT::getNextGOTPLTEntry()
{
  return ++m_GOTPLTIterator;
}

ARMGOT::iterator ARMGOT::getGOTPLTBegin()
{
  // Move to the first GOTPLT entry from last GOT0 entry.
  iterator begin = m_GOTPLTBegin;
  return ++begin;
}

const ARMGOT::iterator ARMGOT::getGOTPLTEnd()
{
  // Move to end or the first normal GOT entry from the last GOTPLT entry.
  iterator end = m_GOTPLTEnd;
  return ++end;
}

uint64_t ARMGOT::emit(MemoryRegion& pRegion)
{
  uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

  GOTEntry* got = 0;
  unsigned int entry_size = getEntrySize();
  uint64_t result = 0x0;
  for (iterator it = begin(), ie = end();
       it != ie; ++it, ++buffer) {
      got = &(llvm::cast<GOTEntry>((*it)));
      *buffer = static_cast<uint32_t>(got->getContent());
      result += entry_size;
  }
  return result;
}

