/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#include <mcld/LD/Relocation.h>
#include <mcld/LD/ResolveInfo.h>

using namespace mcld;

Relocation::Relocation(Relocation::Type pType,
                       Relocation::DWord& pTargetData,
                       const MCFragmentRef& pTargetRef,
                       Relocation::Address pAddend,
		       const ResolveInfo& pSymInfo)
  : m_Type(pType),
    m_Target(pTargetData),
    m_TargetAddress(pTargetRef),
    m_Addend(pAddend),
    m_pSymInfo(&pSymInfo){
}

Relocation::~Relocation()
{
}

Relocation::Address Relocation::place() const
{
  return 0; // TODO
}

