//===- Relocation.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/Relocation.h>
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/Layout.h>

using namespace mcld;

Relocation::Relocation(Relocation::Type pType,
                       const MCFragmentRef& pTargetRef,
                       Relocation::Address pAddend,
                       Relocation::DWord* pTargetData,
                       RelocationFactory &pParent)
  : m_Type(pType),
    m_TargetAddress(pTargetRef),
    m_Addend(pAddend),
    m_pTargetData(pTargetData),
    m_Parent(pParent) {
}

Relocation::~Relocation()
{
}

Relocation::Address Relocation::place(const Layout& pLayout) const
{
  return 0; // TODO
}

void Relocation::apply()
{
  m_Parent.applyRelocation(*this);
}

