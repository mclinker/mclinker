//===- Relocation.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/MC/MCAssembler.h>
//#include <mcld/MC/MCLDInfo.h>
#include <mcld/LD/Relocation.h>
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LD/Layout.h>

using namespace mcld;

Relocation::Relocation(Relocation::Type pType,
                       MCFragmentRef* pTargetRef,
                       Relocation::Address pAddend,
                       Relocation::DWord* pTargetData,
                       RelocationFactory &pParent)
  : MCFragment(llvm::MCFragment::FT_Reloc),
    m_Type(pType),
    m_Addend(pAddend),
    m_pTargetData(pTargetData),
    m_pSymInfo(0),
    m_Parent(pParent)
{
  if(NULL != pTargetRef)
     m_TargetAddress.assign(*pTargetRef->frag(), pTargetRef->offset()) ;
}

Relocation::~Relocation()
{
}

Relocation::Address Relocation::place(const Layout& pLayout) const
{
  Address sect_addr = pLayout.getOutputLDSection(*(m_TargetAddress.frag()))->addr();
  return sect_addr + pLayout.getOutputOffset(m_TargetAddress);
}

Relocation::Address Relocation::symValue() const
{
  if(m_pSymInfo->type() == ResolveInfo::Section) {
    return llvm::cast<LDSection>(
      m_pSymInfo->outSymbol()->fragRef()->frag()->getParent()->getSection()).addr();
  }
  return m_pSymInfo->outSymbol()->value();
}

void Relocation::apply(const MCLDInfo& pLDInfo)
{
  m_Parent.applyRelocation(*this, pLDInfo);
}

void Relocation::setType(Type pType)
{
  m_Type = pType;
}

void Relocation::setAddend(Address pAddend)
{
  m_Addend = pAddend;
}

void Relocation::setSymInfo(ResolveInfo* pSym)
{
  m_pSymInfo = pSym;
}

Relocation::DWord& Relocation::target()
{
  assert(0 != m_pTargetData);
  return *m_pTargetData;
}

const Relocation::DWord& Relocation::target() const
{
  assert(0 != m_pTargetData);
  return *m_pTargetData;
}

