//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/Casting.h>

#include <mcld/Target/GOT.h>
#include <mcld/Support/MsgHandling.h>

#include <cstring>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GOTEntry
//===----------------------------------------------------------------------===//
GOTEntry::GOTEntry(uint64_t pContent, size_t pEntrySize, SectionData* pParent)
  : TargetFragment(Fragment::Target, pParent),
    f_Content(pContent),
    m_EntrySize(pEntrySize) {
}

GOTEntry::~GOTEntry()
{
}

//===----------------------------------------------------------------------===//
// GOT
//===----------------------------------------------------------------------===//
GOT::GOT(LDSection& pSection,
         SectionData& pSectionData,
         size_t pEntrySize)
  : m_Section(pSection),
    m_SectionData(pSectionData),
    f_EntrySize(pEntrySize),
    m_GOTIterator(),
    m_fIsVisit(false) {
}

GOT::~GOT()
{
}

size_t GOT::getEntrySize() const
{
  return f_EntrySize;
}

GOT::iterator GOT::begin()
{
  return m_SectionData.getFragmentList().begin();
}

GOT::const_iterator GOT::begin() const
{
  return m_SectionData.getFragmentList().begin();
}

GOT::iterator GOT::end()
{
  return m_SectionData.getFragmentList().end();
}

GOT::const_iterator GOT::end() const
{
  return m_SectionData.getFragmentList().end();
}

void GOT::reserveEntry(size_t pNum)
{
  GOTEntry* Entry = NULL;

  for (size_t i = 0; i < pNum; i++) {
    Entry = new (std::nothrow) GOTEntry(0, f_EntrySize,
                                        &m_SectionData);
    if (!Entry)
      fatal(diag::fail_allocate_memory_got);
  }
}

GOTEntry* GOT::getOrConsumeEntry(const ResolveInfo& pInfo, bool& pExist)
{
  GOTEntry *&Entry = m_SymEntryMap[&pInfo];
  pExist = 1;

  if (!Entry) {
    pExist = 0;
    assert(m_GOTIterator != m_SectionData.getFragmentList().end()
             && "The number of GOT Entries and ResolveInfo doesn't match!");
    Entry = consumeEntry();
  }
  return Entry;
}

GOTEntry* GOT::consumeEntry()
{
  // first time get GOT entry, set m_GOTIterator
  if(!m_fIsVisit) {
    assert( !m_SectionData.getFragmentList().empty() &&
             "GOT Section contains no entries.");
    m_GOTIterator = m_SectionData.getFragmentList().begin();
    m_fIsVisit = true;
    return &llvm::cast<GOTEntry>(*m_GOTIterator);
  }

  ++m_GOTIterator;
  GOTEntry& entry = llvm::cast<GOTEntry>(*m_GOTIterator);
  return &entry;
}

void GOT::finalizeSectionSize()
{
  m_Section.setSize(m_SectionData.size() * f_EntrySize);
}

