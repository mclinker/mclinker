//===- X86GOTPLT.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86GOTPLT.h"
#include "mcld/LD/LDFileFormat.h"
#include <llvm/Support/ErrorHandling.h>
#include <new>

namespace {
  const uint64_t X86GOTPLTEntrySize = 4;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// X86GOTPLT
X86GOTPLT::X86GOTPLT(LDSection& pSection, llvm::MCSectionData& pSectionData)
  : GOT(pSection, pSectionData, X86GOTPLTEntrySize), m_GOTPLTIterator()
{
  GOTEntry* Entry = 0;

  // Create GOT0 entries.
  for (int i = 0; i < 3; i++) {
    Entry = new (std::nothrow) GOTEntry(0, X86GOTPLTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      llvm::report_fatal_error("Allocating GOT0 entries failed!");

    m_Section.setSize(m_Section.size() + X86GOTPLTEntrySize);
  }

  // Skip GOT0 entries.
  iterator it = m_SectionData.begin();
  iterator ie = m_SectionData.end();

  for (size_t i = 1; i < X86GOT0Num; ++i) {
    if (it == ie)
      llvm::report_fatal_error("Generation of GOT0 entries is incomplete!");

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

void X86GOTPLT::applyGOT0(const uint64_t pAddress)
{
  llvm::cast<GOTEntry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

void X86GOTPLT::reserveGOTPLTEntry()
{
    GOTEntry* got_entry = 0;

    got_entry= new GOTEntry(0, getEntrySize(),&(getSectionData()));

    if (!got_entry)
      llvm::report_fatal_error("Allocating new memory for GOT failed!");

    m_Section.setSize(m_Section.size() + getEntrySize());
}

void X86GOTPLT::applyAllGOTPLT(const uint64_t pPLTBase)
{
  iterator gotplt_it = begin();
  iterator gotplt_ie = end();

  for (; gotplt_it != gotplt_ie; ++gotplt_it)
    llvm::cast<GOTEntry>(*gotplt_it).setContent(pPLTBase);
}

GOTEntry*& X86GOTPLT::lookupGOTPLTMap(const ResolveInfo& pSymbol)
{
  return m_GOTPLTMap[&pSymbol];
}

X86GOTPLT::iterator X86GOTPLT::getNextGOTPLTEntry()
{
  return ++m_GOTPLTIterator;
}

} //end mcld namespace
