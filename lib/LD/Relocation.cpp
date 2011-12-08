/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#include <mcld/LD/Relocation.h>
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/Layout.h>

using namespace mcld;

Relocation::Relocation(Relocation::Type pType,
                       Relocation::Pointer pApply,
                       const MCFragmentRef& pTargetRef,
                       Relocation::Address pAddend,
		       Relocation::DWord pTarget,
		       RelocationFactory &pFactory
		       )
  : m_Type(pType),
    m_pApply(pApply),    
    m_TargetAddress(pTargetRef),
    m_Addend(pAddend),
    m_Target(pTarget),
    m_pFactory(&pFactory){
}

Relocation::~Relocation()
{
}

Relocation::Address Relocation::place(Layout& pLayout) const
{
  return 0; // TODO
}

