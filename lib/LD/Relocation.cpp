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
#include <mcld/LD/Layout.h>
#include <mcld/Support/MsgHandling.h>

using namespace mcld;

Relocation::Relocation(Relocation::Type pType,
                       FragmentRef* pTargetRef,
                       Relocation::Address pAddend,
                       Relocation::DWord pTargetData)
  : Fragment(Fragment::Relocation),
    m_Type(pType),
    m_TargetData(pTargetData),
    m_pSymInfo(NULL),
    m_Addend(pAddend)
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
  if (m_pSymInfo->type() == ResolveInfo::Section &&
     m_pSymInfo->outSymbol()->hasFragRef()) {
    return m_pSymInfo->outSymbol()->fragRef()->frag()->getParent()->getSection().addr();
  }
  return m_pSymInfo->outSymbol()->value();
}

void Relocation::apply(RelocationFactory& pRelocFactory,
                       const MCLDInfo& pLDInfo)
{
  RelocationFactory::Result result =
                                 pRelocFactory.applyRelocation(*this, pLDInfo);

  switch (result) {
    case RelocationFactory::OK: {
      // do nothing
      return;
    }
    case RelocationFactory::Overflow: {
      error(diag::result_overflow) << pRelocFactory.getName(type())
                                   << symInfo()->name();
      return;
    }
    case RelocationFactory::BadReloc: {
      error(diag::result_badreloc) << pRelocFactory.getName(type())
                                   << symInfo()->name();
      return;
    }
    case RelocationFactory::Unsupport: {
      fatal(diag::unsupported_relocation) << type()
                                          << "mclinker@googlegroups.com";
      return;
    }
  } // end of switch
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
  return m_TargetData;
}

const Relocation::DWord& Relocation::target() const
{
  return m_TargetData;
}

