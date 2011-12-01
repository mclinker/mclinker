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

Relocation::Relocation(const Howto& pHowto,
                       const MCFragmentRef& pTarget,
                       Relocation::Address pAddend)
  : m_pHowto(&pHowto),
    m_FragmentRef(pTarget),
    m_Addend(pAddend) {
}

Relocation::~Relocation()
{
}

Relocation::Type Relocation::type() const
{
}

Relocation::Address Relocation::place() const
{
}

