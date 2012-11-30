//===- Relocation.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Fragment/Relocation.h>
#include <mcld/LD/Relocator.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Support/MsgHandling.h>

using namespace mcld;

Relocation::Relocation()
  : m_Type(0x0), m_TargetData(0x0), m_pSymInfo(NULL), m_Addend(0x0) {
}

Relocation::Relocation(Relocation::Type pType,
                       FragmentRef* pTargetRef,
                       Relocation::Address pAddend,
                       Relocation::DWord pTargetData)
  : m_Type(pType),
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

Relocation::Address Relocation::place() const
{
  Address sect_addr = m_TargetAddress.frag()->getParent()->getSection().addr();
  return sect_addr + m_TargetAddress.getOutputOffset();
}

Relocation::Address Relocation::symValue() const
{
  if (m_pSymInfo->type() == ResolveInfo::Section &&
     m_pSymInfo->outSymbol()->hasFragRef()) {
    return m_pSymInfo->outSymbol()->fragRef()->frag()->getParent()->getSection().addr();
  }
  return m_pSymInfo->outSymbol()->value();
}

void Relocation::apply(Relocator& pRelocator)
{
  Relocator::Result result = pRelocator.applyRelocation(*this);

  switch (result) {
    case Relocator::OK: {
      // do nothing
      return;
    }
    case Relocator::Overflow: {
      error(diag::result_overflow) << pRelocator.getName(type())
                                   << symInfo()->name();
      return;
    }
    case Relocator::BadReloc: {
      error(diag::result_badreloc) << pRelocator.getName(type())
                                   << symInfo()->name();
      return;
    }
    case Relocator::Unsupport: {
      fatal(diag::unsupported_relocation) << type()
                                          << "mclinker@googlegroups.com";
      return;
    }
    case Relocator::Unknown: {
      fatal(diag::unknown_relocation) << type() << symInfo()->name();
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

size_t Relocation::size() const
{
  // TODO: the size of Relocation fragment is handled by backend
  return 0;
}

void Relocation::updateAddend()
{
  // Update value keep in addend if we meet a section symbol
  if (m_pSymInfo->type() == ResolveInfo::Section) {
    uint32_t offset = m_pSymInfo->outSymbol()->fragRef()->getOutputOffset();
    m_Addend += offset;
  }
}

