//===- MipsDynRelSection.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDSection.h>
#include "MipsDynRelSection.h"

using namespace mcld;

//==========================
// MipsDynRelSection

MipsDynRelSection::MipsDynRelSection(LDSection& pSection,
                                     llvm::MCSectionData& pSectionData,
                                     const unsigned int pEntrySize)
  : m_pSection(&pSection),
    m_pSectionData(&pSectionData),
    m_EntryBytes(pEntrySize) {

}

void MipsDynRelSection::reserveEntry(RelocationFactory& pRelFactory,
                                     int pNum)
{
  m_pSectionData->getFragmentList().push_back(pRelFactory.produceEmptyEntry());
  // update section size
  m_pSection->setSize(m_pSection->size() + m_EntryBytes);
}

Relocation* MipsDynRelSection::getEntry(const ResolveInfo& pSymbol,
                                        bool& pExist)
{
  // first time visit this function, set m_pEmpty to Fragments.begin()
  if (m_SymRelMap.empty()) {
    assert(!m_pSectionData->getFragmentList().empty() &&
           "DynRelSection contains no entries.");
    m_pEmpty = m_pSectionData->getFragmentList().begin();
  }

  // get or create entry in m_SymRelMap
  Relocation *&Entry = m_SymRelMap[&pSymbol];
  pExist = true;

  if (!Entry) {
    pExist = false;
    assert(m_pEmpty != m_pSectionData->end() &&
           "No empty relocation entry for the incoming symbol.");
    Entry = llvm::cast<Relocation>(&(*m_pEmpty));
    ++m_pEmpty;
  }

  return Entry;
}
