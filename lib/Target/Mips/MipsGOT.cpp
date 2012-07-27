//===- MipsGOT.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MipsGOT.h"

#include <llvm/Support/Casting.h>

#include <mcld/LD/ResolveInfo.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>

namespace {
  const size_t MipsGOTEntrySize = 4;
  const size_t MipsGOT0Num = 1;
}

using namespace mcld;

//===----------------------------------------------------------------------===//
// MipsGOT
MipsGOT::MipsGOT(LDSection& pSection, SectionData& pSectionData)
  : GOT(pSection, pSectionData, MipsGOTEntrySize),
    m_pLocalNum(0)
{
  // Create GOT0 entries.
  for (size_t i = 0; i < MipsGOT0Num; ++i) {
    GOTEntry* entry =
      new (std::nothrow) GOTEntry(0, MipsGOTEntrySize, &m_SectionData);

    if (NULL == entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + MipsGOTEntrySize);
  }

  // Skip GOT0 entries.
  iterator it = m_SectionData.begin();

  for (size_t i = 1; i < MipsGOT0Num; ++i) {
    assert((it != m_SectionData.end()) &&
           "Generation of GOT0 entries is incomplete!");

    ++it;
  }

  m_LocalGOTIterator = it;
  m_GlobalGOTIterator = it;
  m_pLocalNum = MipsGOT0Num;
}

MipsGOT::iterator MipsGOT::begin()
{
  return m_SectionData.getFragmentList().begin();
}

MipsGOT::iterator MipsGOT::end()
{
  return m_SectionData.getFragmentList().end();
}

MipsGOT::const_iterator MipsGOT::begin() const
{
  return m_SectionData.getFragmentList().begin();
}

MipsGOT::const_iterator MipsGOT::end() const
{
  return m_SectionData.getFragmentList().end();
}

uint64_t MipsGOT::emit(MemoryRegion& pRegion)
{
  uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

  size_t entry_size = getEntrySize();

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
  for (size_t i = 0; i < pNum; ++i) {
    GOTEntry* entry =
      new (std::nothrow) GOTEntry(0, MipsGOTEntrySize, &m_SectionData);

    if (NULL == entry)
      fatal(diag::fail_allocate_memory_got);

    m_Section.setSize(m_Section.size() + MipsGOTEntrySize);
  }
}

void MipsGOT::reserveLocalEntry()
{
  reserveEntry(1);
  ++m_pLocalNum;

  // Move global entries iterator forward.
  // We need to put global GOT entries after all local ones.
  ++m_GlobalGOTIterator;
}

void MipsGOT::reserveGlobalEntry()
{
  reserveEntry(1);
}

GOTEntry* MipsGOT::getEntry(const ResolveInfo& pInfo, bool& pExist)
{
  if (isLocal(&pInfo) && pInfo.type() == ResolveInfo::Section) {
    pExist = false;
    iterator& it = m_LocalGOTIterator;
    ++it;
    assert(it != m_SectionData.getFragmentList().end() &&
           "The number of GOT Entries and ResolveInfo doesn't match");
    GOTEntry* entry = llvm::cast<GOTEntry>(&(*it));
    return entry;
  }

  GOTEntry*& entry = m_GeneralGOTMap[&pInfo];

  pExist = NULL != entry;

  if (!pExist) {
    iterator& it = isLocal(&pInfo)  ? m_LocalGOTIterator : m_GlobalGOTIterator;

    ++it;

    assert(it != m_SectionData.getFragmentList().end() &&
           "The number of GOT Entries and ResolveInfo doesn't match");

    entry = llvm::cast<GOTEntry>(&(*it));
  }

  return entry;
}

size_t MipsGOT::getTotalNum() const
{
  return m_SectionData.getFragmentList().size();
}

size_t MipsGOT::getLocalNum() const
{
  return m_pLocalNum;
}

