//===- MipsGOTPLT.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/Casting.h>
#include "MipsGOTPLT.h"

namespace {
  typedef mcld::GOT::Entry<4> GOTPLTEntry;

  const size_t MipsGOTPLT0Num = 2;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// MipsGOTPLT
//===----------------------------------------------------------------------===//
MipsGOTPLT::MipsGOTPLT(LDSection& pSection)
  : GOT(pSection)
{
  // Create header's entries.
  new GOTPLTEntry(0, m_SectionData);
  new GOTPLTEntry(0, m_SectionData);
  m_Last = ++m_SectionData->begin();
}

void MipsGOTPLT::reserve(size_t pNum)
{
  for (size_t i = 0; i < pNum; i++)
    new GOTPLTEntry(0, m_SectionData);
}

uint64_t MipsGOTPLT::emit(MemoryRegion& pRegion)
{
  uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.begin());

  uint64_t result = 0;
  for (iterator it = begin(), ie = end(); it != ie; ++it, ++buffer) {
    GOTPLTEntry* got = &(llvm::cast<GOTPLTEntry>((*it)));
    *buffer = static_cast<uint32_t>(got->getValue());
    result += got->size();
  }
  return result;
}

Fragment* MipsGOTPLT::consume()
{
  ++m_Last;
  assert(m_Last != m_SectionData->end() &&
         "There is no reserved GOTPLT entries");
  return &(*m_Last);
}

bool MipsGOTPLT::hasGOT1() const
{
  return m_SectionData->size() > MipsGOTPLT0Num;
}

uint64_t MipsGOTPLT::getEntryAddr(size_t num) const
{
  return addr() + (MipsGOTPLT0Num + num) * GOTPLTEntry::EntrySize;
}

void MipsGOTPLT::applyAllGOTPLT(uint64_t pltAddr)
{
  iterator it = begin();
  llvm::cast<GOTPLTEntry>(*it++).setValue(0); // PLT lazy resolver
  llvm::cast<GOTPLTEntry>(*it++).setValue(0); // Module pointer

  for (; it != end(); ++it)
    llvm::cast<GOTPLTEntry>(*it).setValue(pltAddr);
}

} //end mcld namespace
