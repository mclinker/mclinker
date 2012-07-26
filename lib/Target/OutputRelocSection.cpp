//===- OutputRelocSection.cpp ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Target/OutputRelocSection.h>

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDSection.h>
#include <mcld/Support/MsgHandling.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// OutputRelocSection
//===----------------------------------------------------------------------===//
OutputRelocSection::OutputRelocSection(LDSection& pSection,
                                       SectionData& pSectionData,
                                       unsigned int pEntrySize)
  : m_pSection(&pSection),
    m_pSectionData(&pSectionData),
    m_EntryBytes(pEntrySize),
    m_isVisit(false),
    m_ValidEntryIterator(){
}

OutputRelocSection::~OutputRelocSection()
{
}

void OutputRelocSection::reserveEntry(RelocationFactory& pRelFactory,
                                      size_t pNum)
{
  for(size_t i=0; i<pNum; i++) {
    m_pSectionData->getFragmentList().push_back(pRelFactory.produceEmptyEntry());
    // update section size
    m_pSection->setSize(m_pSection->size() + m_EntryBytes);
  }
}

Relocation* OutputRelocSection::getEntry(const ResolveInfo& pSymbol,
                                         bool isForGOT,
                                         bool& pExist)
{
  // first time visit this function, set m_ValidEntryIterator to
  // Fragments.begin()
  if(!m_isVisit) {
    assert( !m_pSectionData->getFragmentList().empty() &&
             "DynRelSection contains no entries.");
    m_ValidEntryIterator = m_pSectionData->getFragmentList().begin();
    m_isVisit = true;
  }

  assert(m_ValidEntryIterator != m_pSectionData->end() &&
         "No empty relocation entry for the incoming symbol.");

  // if this relocation is used to relocate GOT (.got or .got.plt),
  // check if we've gotten an entry for this symbol before. If yes,
  // return the found entry in map.
  // Otherwise, this relocation is used to relocate general section
  // (data or text section), return an empty entry directly.
  Relocation* result;

  if(isForGOT) {
    // get or create entry in m_SymRelMap
    Relocation *&entry = m_SymRelMap[&pSymbol];
    pExist = true;

    if(NULL == entry) {
      pExist = false;
      entry = llvm::cast<Relocation>(&(*m_ValidEntryIterator));
      ++m_ValidEntryIterator;
    }
    result = entry;
  }
  else {
    pExist = false;
    result = llvm::cast<Relocation>(&(*m_ValidEntryIterator));
    ++m_ValidEntryIterator;
  }
  return result;
}

