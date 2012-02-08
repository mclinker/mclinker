//===- MipsGOT.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/Support/ErrorHandling.h>
#include <mcld/Support/MemoryRegion.h>
#include "MipsGOT.h"

namespace {
  const uint64_t MipsGOTEntrySize = 4;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// MipsGOT
MipsGOT::MipsGOT(LDSection& pSection, llvm::MCSectionData& pSectionData)
  : GOT(pSection, pSectionData, MipsGOTEntrySize)
{
}

MipsGOT::iterator MipsGOT::begin()
{
  return m_SectionData.getFragmentList().begin();
}

MipsGOT::iterator MipsGOT::end()
{
  return m_SectionData.getFragmentList().begin();
}

MipsGOT::const_iterator MipsGOT::begin() const
{
  return m_SectionData.getFragmentList().end();
}

MipsGOT::const_iterator MipsGOT::end() const
{
  return m_SectionData.getFragmentList().end();
}

uint64_t MipsGOT::emit(MemoryRegion& pRegion)
{
  uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

  const uint64_t entry_size = getEntrySize();

  uint64_t result = 0;
  for (iterator it = begin(), ie = end();
       it != ie; ++it, ++buffer) {
    GOTEntry* got = &(llvm::cast<GOTEntry>((*it)));
    *buffer = static_cast<uint32_t>(got->getContent());
    result += entry_size;
  }
  return result;
}

void MipsGOT::reserveEntry(size_t pNum)
{
  GOTEntry* Entry = 0;

  for (size_t i = 0; i < pNum; ++i) {
    Entry = new (std::nothrow) GOTEntry(0, MipsGOTEntrySize, &m_SectionData);

    if (!Entry)
      llvm::report_fatal_error("Allocating new GOTEntry failed");

    m_SectionData.getFragmentList().push_back(Entry);
  }
}

GOTEntry* MipsGOT::getEntry(const ResolveInfo& pInfo, bool& pExist)
{
  GOTEntry*& Entry = m_GeneralGOTMap[&pInfo];

  pExist = Entry != 0;

  if (!pExist) {
    ++m_GeneralGOTIterator;

    assert(m_GeneralGOTIterator != m_SectionData.getFragmentList().end() &&
           "The number of GOT Entries and ResolveInfo doesn't match");

    Entry = llvm::cast<GOTEntry>(&(*m_GeneralGOTIterator));
  }

  return Entry;
}

}
