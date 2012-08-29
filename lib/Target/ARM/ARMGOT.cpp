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
  const unsigned int ARMGOT0Num = 3;
} // end of anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMGOT
ARMGOT::ARMGOT(LDSection& pSection, SectionData& pSectionData)
             : GOT(pSection, pSectionData, ARMGOTEntrySize),
               m_GOTPLTIterator(),
               m_GOTPLTBegin(),
               m_GOTPLTEnd()
{
  // Create GOT0 entries.
  reserveEntry(ARMGOT0Num);

  // Skip GOT0 entries.
  for (int i = 0; i < ARMGOT0Num; ++i) {
    consumeEntry();
  }

  // initialize GOTPLT iterator
  m_GOTPLTIterator = m_GOTIterator;
  m_GOTPLTBegin = m_GOTIterator;
  m_GOTPLTEnd = m_GOTIterator;
}

ARMGOT::~ARMGOT()
{
}

void ARMGOT::reserveGOTPLTEntry()
{
    GOTEntry* got_entry = 0;

    got_entry= new GOTEntry(0, getEntrySize(),&(getSectionData()));

    if (!got_entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + getEntrySize());

    ++m_GOTPLTEnd;
    // m_GOTIterator point to the normal GOT entry
    ++m_GOTIterator;
}

GOTEntry* ARMGOT::getOrConsumeGOTPLTEntry(const ResolveInfo& pInfo,
                                          bool& pExist)
{
  GOTEntry *&entry = m_GOTPLTMap[&pInfo];
  pExist = 1;

  if (!entry) {
    pExist = 0;
    assert(m_GOTIterator != m_SectionData.getFragmentList().end()
             && "The number of GOT Entries and ResolveInfo doesn't match!");
    ++m_GOTPLTIterator;
    entry = llvm::cast<GOTEntry>(&(*m_GOTPLTIterator));
  }
  return entry;
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

