//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86GOT.h"
#include <mcld/LD/LDFileFormat.h>
#include <llvm/Support/ErrorHandling.h>
#include <new>

namespace {
  const uint64_t X86GOTEntrySize = 4;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// X86GOT
X86GOT::X86GOT(LDSection& pSection, llvm::MCSectionData& pSectionData)
             : GOT(pSection, pSectionData, X86GOTEntrySize),
               m_GeneralGOTNum(0), m_GOTPLTNum(0), m_GeneralGOTIterator(),
               m_GOTPLTIterator(), m_LastGOT0()
{
  GOTEntry* Entry = 0;

  // Create GOT0 entries.
  for (int i = 0; i < X86GOT0Num; i++) {
    Entry = new (std::nothrow) GOTEntry(0, X86GOTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      llvm::report_fatal_error("Allocating GOT0 entries failed!");

    m_Section.setSize(m_Section.size() + X86GOTEntrySize);
  }

  // Skip GOT0 entries.
  iterator it = m_SectionData.begin();
  iterator ie = m_SectionData.end();

  for (int i = 1; i < X86GOT0Num; ++i) {
    if (it == ie)
      llvm::report_fatal_error("Generation of GOT0 entries is incomplete!");

    ++it;
  }

  m_LastGOT0 = it;
  m_GeneralGOTIterator = it;
  m_GOTPLTIterator = it;
}

X86GOT::~X86GOT()
{
}

void X86GOT::reserveEntry(size_t pNum)
{
  GOTEntry* Entry = 0;

  for (int i = 0; i < pNum; i++) {
    Entry = new (std::nothrow) GOTEntry(0, X86GOTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      llvm::report_fatal_error("Allocating new memory for GOTEntry failed");

    m_Section.setSize(m_Section.size() + X86GOTEntrySize);
    ++m_GeneralGOTNum;
  }
}


GOTEntry* X86GOT::getEntry(const ResolveInfo& pInfo, bool& pExist)
{
  GOTEntry *&Entry = m_GeneralGOTMap[&pInfo];
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

void X86GOT::applyGOT0(uint64_t pAddress)
{
  llvm::cast<GOTEntry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

X86GOT::iterator X86GOT::begin()
{
  return m_SectionData.getFragmentList().begin();
}

X86GOT::const_iterator X86GOT::begin() const
{
  return m_SectionData.getFragmentList().begin();
}

X86GOT::iterator X86GOT::end()
{
  return m_SectionData.getFragmentList().end();
}

X86GOT::const_iterator X86GOT::end() const
{
  return m_SectionData.getFragmentList().end();
}

unsigned int X86GOT::getGOTPLTNum() const
{ return m_GOTPLTNum; }

X86GOT::iterator X86GOT::getLastGOT0()
{ return m_LastGOT0; }

const X86GOT::iterator X86GOT::getLastGOT0() const
{ return m_LastGOT0; }

} //end mcld namespace
