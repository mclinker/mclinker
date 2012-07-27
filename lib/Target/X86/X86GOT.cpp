//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86GOT.h"

#include <new>

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Support/MsgHandling.h>

namespace {
  const size_t X86GOTEntrySize = 4;
}

using namespace mcld;

//===----------------------------------------------------------------------===//
// X86GOT
//===----------------------------------------------------------------------===//
X86GOT::X86GOT(LDSection& pSection, SectionData& pSectionData)
             : GOT(pSection, pSectionData, X86GOTEntrySize),
               m_GOTIterator(), m_fIsVisit(false)
{
}

X86GOT::~X86GOT()
{
}

void X86GOT::reserveEntry(size_t pNum)
{
  GOTEntry* Entry = 0;

  for (size_t i = 0; i < pNum; i++) {
    Entry = new (std::nothrow) GOTEntry(0, X86GOTEntrySize,
                                        &m_SectionData);

    if (!Entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + X86GOTEntrySize);
  }
}


GOTEntry* X86GOT::getEntry(const ResolveInfo& pInfo, bool& pExist)
{
  // first time visit this function, set m_GOTIterator
  if(!m_fIsVisit) {
    assert( !m_SectionData.getFragmentList().empty() &&
             "DynRelSection contains no entries.");
    m_GOTIterator = m_SectionData.getFragmentList().begin();
    m_fIsVisit = true;
  }


  GOTEntry *&Entry = m_GOTMap[&pInfo];
  pExist = 1;

  if (!Entry) {
    pExist = 0;
    assert(m_GOTIterator != m_SectionData.getFragmentList().end()
             && "The number of GOT Entries and ResolveInfo doesn't match!");
    Entry = llvm::cast<GOTEntry>(&(*m_GOTIterator));
    ++m_GOTIterator;
  }
  return Entry;
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

