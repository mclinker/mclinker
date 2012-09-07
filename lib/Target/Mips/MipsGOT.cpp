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
  reserveEntry(MipsGOT0Num);

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

bool MipsGOT::hasGOT1() const
{
  return (m_SectionData.size() > MipsGOT0Num);
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

GOTEntry* MipsGOT::getOrConsumeEntry(const ResolveInfo& pInfo, bool& pExist)
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

  GOTEntry*& entry = m_SymEntryMap[&pInfo];

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

