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

#include <mcld/LD/LDSection.h>
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
  : GOT(pSection, pSectionData, ARMGOTEntrySize)
{
  // Create GOT0 entries.
  reserve(ARMGOT0Num);

  // Skip GOT0 entries.
  for (int i = 0; i < ARMGOT0Num; ++i) {
    consume();
  }

  // initialize GOTPLT iterator
  m_GOTPLTIterator = m_GOTIterator;
  m_GOTPLTBegin = m_GOTIterator;
  m_GOTPLTEnd = m_GOTIterator;
}

ARMGOT::~ARMGOT()
{
}

bool ARMGOT::hasGOT1() const
{
  return (m_SectionData.size() > ARMGOT0Num);
}

void ARMGOT::reserveGOTPLTEntry()
{
    Entry* got_entry= new Entry(0, getEntrySize(), &m_SectionData);

    if (!got_entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + getEntrySize());

    ++m_GOTPLTEnd;
    // m_GOTIterator point to the normal GOT entry
    ++m_GOTIterator;
}

GOT::Entry* ARMGOT::consumeGOTPLTEntry()
{
  assert(m_GOTIterator != m_SectionData.getFragmentList().end() &&
         "The number of GOT Entries and ResolveInfo doesn't match!");
  ++m_GOTPLTIterator;
  return llvm::cast<GOT::Entry>(&(*m_GOTPLTIterator));
}

void ARMGOT::applyGOT0(uint64_t pAddress)
{
  llvm::cast<Entry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

void ARMGOT::applyAllGOTPLT(uint64_t pPLTBase)
{
  iterator begin = getGOTPLTBegin();
  iterator end = getGOTPLTEnd();

  for (;begin != end ;++begin)
    llvm::cast<Entry>(*begin).setContent(pPLTBase);
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

  Entry* got = 0;
  unsigned int entry_size = getEntrySize();
  uint64_t result = 0x0;
  for (iterator it = begin(), ie = end();
       it != ie; ++it, ++buffer) {
      got = &(llvm::cast<Entry>((*it)));
      *buffer = static_cast<uint32_t>(got->getContent());
      result += entry_size;
  }
  return result;
}

