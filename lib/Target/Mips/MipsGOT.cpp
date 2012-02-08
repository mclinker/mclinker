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
  const size_t MipsGOT0Num = 1;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// MipsGOT
MipsGOT::MipsGOT(LDSection& pSection, llvm::MCSectionData& pSectionData)
  : GOT(pSection, pSectionData, MipsGOTEntrySize),
    m_pLocalNum(0)
{
  // Create GOT0 entries.
  for (size_t i = 0; i < MipsGOT0Num; ++i) {
    GOTEntry* entry =
      new (std::nothrow) GOTEntry(0, MipsGOTEntrySize, &m_SectionData);

    if (NULL == entry)
      llvm::report_fatal_error("Allocating GOT0 entries failed!");

    m_Section.setSize(m_Section.size() + MipsGOTEntrySize);
  }

  // Skip GOT0 entries.
  iterator it = m_SectionData.begin();
  iterator ie = m_SectionData.end();

  for (size_t i = 1; i < MipsGOT0Num; ++i) {
    if (it == ie)
      llvm::report_fatal_error("Generation of GOT0 entries is incomplete!");

    ++it;
  }

  m_GeneralGOTIterator = it;
  m_pLocalNum = MipsGOT0Num;
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
    GOTEntry* entry =
      new (std::nothrow) GOTEntry(0, MipsGOTEntrySize, &m_SectionData);

    if (NULL == entry)
      llvm::report_fatal_error("Allocating new GOTEntry failed");

    m_SectionData.getFragmentList().push_back(entry);
  }
}

GOTEntry* MipsGOT::getEntry(const ResolveInfo& pInfo, bool& pExist)
{
  GOTEntry*& entry = m_GeneralGOTMap[&pInfo];

  pExist = NULL != entry;

  if (!pExist) {
    ++m_GeneralGOTIterator;

    assert(m_GeneralGOTIterator != m_SectionData.getFragmentList().end() &&
           "The number of GOT Entries and ResolveInfo doesn't match");

    entry = llvm::cast<GOTEntry>(&(*m_GeneralGOTIterator));
  }

  return entry;
}

size_t MipsGOT::getLocalNum() const
{
  return m_pLocalNum;
}

}
