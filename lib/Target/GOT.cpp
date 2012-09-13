//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/Casting.h>

#include <mcld/LD/LDSection.h>
#include <mcld/Target/GOT.h>
#include <mcld/Support/MsgHandling.h>

#include <cstring>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GOT::Entry
//===----------------------------------------------------------------------===//
GOT::Entry::Entry(uint64_t pContent, size_t pEntrySize, SectionData* pParent)
  : TargetFragment(Fragment::Target, pParent),
    f_Content(pContent),
    m_EntrySize(pEntrySize) {
}

GOT::Entry::~Entry()
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

void GOT::reserve(size_t pNum)
{
  Entry* entry = NULL;

  for (size_t i = 0; i < pNum; i++) {
    entry = new Entry(0, f_EntrySize, &m_SectionData);
  }
}

GOT::Entry* GOT::getOrConsumeEntry(const ResolveInfo& pInfo, bool& pExist)
{
  Entry *&entry = m_SymEntryMap[&pInfo];
  pExist = 1;

  if (NULL == entry) {
    pExist = 0;
    assert(m_GOTIterator != m_SectionData.getFragmentList().end()
             && "The number of GOT Entries and ResolveInfo doesn't match!");
    entry = consume();
  }
  return entry;
}

GOT::Entry* GOT::consume()
{
  // first time get GOT entry, set m_GOTIterator
  if(!m_fIsVisit) {
    assert( !m_SectionData.getFragmentList().empty() &&
             "GOT Section contains no entries.");
    m_GOTIterator = m_SectionData.getFragmentList().begin();
    m_fIsVisit = true;
    return &llvm::cast<Entry>(*m_GOTIterator);
  }

  ++m_GOTIterator;
  Entry& entry = llvm::cast<Entry>(*m_GOTIterator);
  return &entry;
}

void GOT::finalizeSectionSize()
{
  m_Section.setSize(m_SectionData.size() * f_EntrySize);
}

