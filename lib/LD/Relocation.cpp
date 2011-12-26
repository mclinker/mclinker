//===- Relocation.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/MC/MCAssembler.h>
#include <mcld/LD/Relocation.h>
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LD/Layout.h>

using namespace mcld;

Relocation::Relocation(Relocation::Type pType,
                       MCFragmentRef* pTargetRef,
                       Relocation::Address pAddend,
                       Relocation::DWord* pTargetData,
                       RelocationFactory &pParent)
  : m_Type(pType),
    m_Addend(pAddend),
    m_pTargetData(pTargetData),
    m_pSymInfo(0),
    m_Parent(pParent),
    MCFragment(llvm::MCFragment::FT_Reloc)
{
  if(pTargetRef)
     m_TargetAddress = *pTargetRef ;
}

Relocation::~Relocation()
{
}

Relocation::Address Relocation::place(const Layout& pLayout) const
{
  Address offset = m_TargetAddress.offset();
  return pLayout.getFragmentOffset(*m_TargetAddress.frag()) + offset;
}

void Relocation::apply()
{
  m_Parent.applyRelocation(*this);
}

void Relocation::setType(Type pType)
{
  m_Type = pType;
}

void Relocation::setAddend(Address pAddend)
{
  m_Addend = pAddend;
}
