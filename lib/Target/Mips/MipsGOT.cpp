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
  const size_t MipsGOT0Num = 1;
}

using namespace mcld;

//===----------------------------------------------------------------------===//
// MipsGOT
//===----------------------------------------------------------------------===//
MipsGOT::MipsGOT(LDSection& pSection)
  : GOT(pSection),
    m_LocalNum(0),
    m_GlobalNum(0),
    m_pLastLocal(NULL),
    m_pLastGlobal(NULL)
{
  // Create GOT0 entries.
  reserve(MipsGOT0Num);
}

void MipsGOT::reserve(size_t pNum)
{
  for (size_t i = 0; i < pNum; i++) {
    new MipsGOTEntry(0, m_SectionData);
  }
}

bool MipsGOT::hasGOT1() const
{
  return (m_LocalNum + m_GlobalNum) > 0;
}

void MipsGOT::finalizeSectionSize()
{
  m_pLastLocal = llvm::cast<MipsGOTEntry>(&m_SectionData->back());
  reserve(m_LocalNum);

  m_pLastGlobal = llvm::cast<MipsGOTEntry>(&m_SectionData->back());
  reserve(m_GlobalNum);

  GOT::finalizeSectionSize();
}

uint64_t MipsGOT::emit(MemoryRegion& pRegion)
{
  uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

  uint64_t result = 0;
  for (iterator it = begin(), ie = end();
       it != ie; ++it, ++buffer) {
    MipsGOTEntry* got = &(llvm::cast<MipsGOTEntry>((*it)));
    *buffer = static_cast<uint32_t>(got->getValue());
    result += got->size();
  }
  return result;
}

void MipsGOT::reserveLocalEntry()
{
  ++m_LocalNum;
}

void MipsGOT::reserveGlobalEntry()
{
  ++m_GlobalNum;
}

MipsGOTEntry* MipsGOT::consumeLocal()
{
  assert(m_LocalNum && "Consume empty local GOT entry!");
  m_pLastLocal = llvm::cast<MipsGOTEntry>(m_pLastLocal->getNextNode());
  return m_pLastLocal;
}

MipsGOTEntry* MipsGOT::consumeGlobal()
{
  assert(m_GlobalNum && "Consume empty global GOT entry!");
  m_pLastGlobal = llvm::cast<MipsGOTEntry>(m_pLastGlobal->getNextNode());
  return m_pLastGlobal;
}

size_t MipsGOT::getTotalNum() const
{
  return getLocalNum() + m_GlobalNum;
}

size_t MipsGOT::getLocalNum() const
{
  return MipsGOT0Num + m_LocalNum;
}

