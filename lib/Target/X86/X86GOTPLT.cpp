//===- X86GOTPLT.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "X86GOTPLT.h"

#include <new>

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDFileFormat.h>
#include <mcld/Support/MsgHandling.h>

namespace {
  const uint64_t X86GOTPLTEntrySize = 4;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// X86GOTPLT
//===----------------------------------------------------------------------===//
X86GOTPLT::X86GOTPLT(LDSection& pSection, SectionData& pSectionData)
  : GOT(pSection, pSectionData, X86GOTPLTEntrySize), m_GOTPLTIterator()
{
  GOTEntry* Entry = 0;

  // Create GOT0 entries.
  for (size_t i = 0; i < X86GOTPLT0Num; i++) {
    Entry = new (std::nothrow) GOTEntry(0, X86GOTPLTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + X86GOTPLTEntrySize);
  }

  // Skip GOT0 entries.
  iterator it = m_SectionData.begin();

  for (size_t i = 1; i < X86GOTPLT0Num; ++i) {
    assert((it != m_SectionData.end()) &&
           "Generation of GOT0 entries is incomplete!");

    ++it;
  }

  m_GOTPLTIterator = it;
}

X86GOTPLT::~X86GOTPLT()
{
}

X86GOTPLT::iterator X86GOTPLT::begin()
{
  return m_SectionData.begin();
}

X86GOTPLT::const_iterator X86GOTPLT::begin() const
{
  return m_SectionData.begin();
}

X86GOTPLT::iterator X86GOTPLT::end()
{
  return m_SectionData.end();
}

X86GOTPLT::const_iterator X86GOTPLT::end() const
{
  return m_SectionData.end();
}

void X86GOTPLT::applyGOT0(uint64_t pAddress)
{
  llvm::cast<GOTEntry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

void X86GOTPLT::reserveEntry(size_t pNum)
{
  GOTEntry* got_entry = NULL;
  for (size_t i = 0; i < pNum; ++i) {
    got_entry = new GOTEntry(0, getEntrySize(),&(getSectionData()));
    if (!got_entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + getEntrySize());
  }
}

void X86GOTPLT::applyAllGOTPLT(uint64_t pPLTBase,
                               unsigned int pPLT0Size,
                               unsigned int pPLT1Size)
{
  iterator it = begin();
  // skip GOT0
  for (size_t i = 0; i < X86GOTPLT0Num; ++i)
    ++it;
  // address of corresponding plt entry
  uint64_t plt_addr = pPLTBase + pPLT0Size;
  for (; it != end() ; ++it) {
    llvm::cast<GOTEntry>(*it).setContent(plt_addr + 6);
    plt_addr += pPLT1Size;
  }
}

GOTEntry*& X86GOTPLT::lookupGOTPLTMap(const ResolveInfo& pSymbol)
{
  return m_GOTPLTMap[&pSymbol];
}

GOTEntry* X86GOTPLT::getEntry(const ResolveInfo& pInfo, bool& pExist)
{
  GOTEntry *&Entry = m_GOTPLTMap[&pInfo];
  pExist = 1;

  if (!Entry) {
    pExist = 0;

    ++m_GOTPLTIterator;
    assert(m_GOTPLTIterator != m_SectionData.getFragmentList().end()
           && "The number of GOT Entries and ResolveInfo doesn't match!");

    Entry = llvm::cast<GOTEntry>(&(*m_GOTPLTIterator));
  }

  return Entry;
}

} //end mcld namespace
