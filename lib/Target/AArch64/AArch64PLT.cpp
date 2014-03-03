//===- AArch64PLT.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "AArch64GOT.h"
#include "AArch64PLT.h"

#include <new>

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDSection.h>
#include <mcld/Support/MsgHandling.h>

using namespace mcld;

AArch64PLT0::AArch64PLT0(SectionData& pParent)
  : PLT::Entry<sizeof(aarch64_plt0)>(pParent) {}

AArch64PLT1::AArch64PLT1(SectionData& pParent)
  : PLT::Entry<sizeof(aarch64_plt1)>(pParent) {}

//===----------------------------------------------------------------------===//
// AArch64PLT

AArch64PLT::AArch64PLT(LDSection& pSection, AArch64GOT &pGOTPLT)
  : PLT(pSection), m_GOT(pGOTPLT) {
  new AArch64PLT0(*m_SectionData);
}

AArch64PLT::~AArch64PLT()
{
}

bool AArch64PLT::hasPLT1() const
{
  return (m_SectionData->size() > 1);
}

void AArch64PLT::finalizeSectionSize()
{
  uint64_t size = (m_SectionData->size() - 1) * sizeof(aarch64_plt1) +
                  sizeof(aarch64_plt0);
  m_Section.setSize(size);

  uint32_t offset = 0;
  SectionData::iterator frag, fragEnd = m_SectionData->end();
  for (frag = m_SectionData->begin(); frag != fragEnd; ++frag) {
    frag->setOffset(offset);
    offset += frag->size();
  }
}

AArch64PLT1* AArch64PLT::create()
{
  AArch64PLT1* plt1_entry = new (std::nothrow) AArch64PLT1(*m_SectionData);
  if (!plt1_entry)
    fatal(diag::fail_allocate_memory_plt);
  return plt1_entry;
}

void AArch64PLT::applyPLT0()
{
}

void AArch64PLT::applyPLT1()
{
}

uint64_t AArch64PLT::emit(MemoryRegion& pRegion)
{
  uint64_t result = 0x0;
  iterator it = begin();

  unsigned char* buffer = pRegion.begin();
  memcpy(buffer, llvm::cast<AArch64PLT0>((*it)).getValue(),
                                                        AArch64PLT0::EntrySize);
  result += AArch64PLT0::EntrySize;
  ++it;

  AArch64PLT1* plt1 = 0;
  AArch64PLT::iterator ie = end();
  while (it != ie) {
    plt1 = &(llvm::cast<AArch64PLT1>(*it));
    memcpy(buffer + result, plt1->getValue(), AArch64PLT1::EntrySize);
    result += AArch64PLT1::EntrySize;
    ++it;
  }
  return result;
}

