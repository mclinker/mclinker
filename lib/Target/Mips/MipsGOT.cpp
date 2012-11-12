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
//===----------------------------------------------------------------------===//
MipsGOT::MipsGOT(LDSection& pSection)
  : GOT(pSection, MipsGOTEntrySize),
    m_pLocalNum(0)
{
  // Create GOT0 entries.
  reserve(MipsGOT0Num);

  // Skip GOT0 entries.
  iterator it = m_SectionData->begin();

  for (size_t i = 1; i < MipsGOT0Num; ++i) {
    assert((it != m_SectionData->end()) &&
           "Generation of GOT0 entries is incomplete!");

    ++it;
  }

  m_LocalGOTIterator = it;
  m_GlobalGOTIterator = it;
  m_pLocalNum = MipsGOT0Num;
}

bool MipsGOT::hasGOT1() const
{
  return (m_SectionData->size() > MipsGOT0Num);
}

uint64_t MipsGOT::emit(MemoryRegion& pRegion)
{
  uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

  size_t entry_size = getEntrySize();

  uint64_t result = 0;
  for (iterator it = begin(), ie = end();
       it != ie; ++it, ++buffer) {
    GOT::Entry* got = &(llvm::cast<GOT::Entry>((*it)));
    *buffer = static_cast<uint32_t>(got->getContent());
    result += entry_size;
  }
  return result;
}

void MipsGOT::reserveLocalEntry()
{
  reserve(1);
  ++m_pLocalNum;

  // Move global entries iterator forward.
  // We need to put global GOT entries after all local ones.
  ++m_GlobalGOTIterator;
}

void MipsGOT::reserveGlobalEntry()
{
  reserve(1);
}

GOT::Entry* MipsGOT::consumeLocal()
{
  iterator& it = m_LocalGOTIterator;
  ++it;
  assert(it != m_SectionData->getFragmentList().end() &&
         "The number of GOT Entries and ResolveInfo doesn't match");
  return llvm::cast<GOT::Entry>(&(*it));
}

GOT::Entry* MipsGOT::consumeGlobal()
{
  iterator& it = m_GlobalGOTIterator;
  ++it;
  assert(it != m_SectionData->getFragmentList().end() &&
         "The number of GOT Entries and ResolveInfo doesn't match");
  return llvm::cast<GOT::Entry>(&(*it));
}

size_t MipsGOT::getTotalNum() const
{
  return m_SectionData->getFragmentList().size();
}

size_t MipsGOT::getLocalNum() const
{
  return m_pLocalNum;
}

