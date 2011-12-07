/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/LD/ResolveInfo.h>
#include <mcld/MC/MCFragmentRef.h>
#include "ARMRelocation.h"

using namespace mcld;

//==========================
// ARMRelocation
ARMRelocation::ARMRelocation(ARMRelocation::Type pType,
                             ARMRelocation::DWord& pTargetData,
                             const MCFragmentRef& pTargetRef,
                             ARMRelocation::Address pAddend,
                             const ResolveInfo& pSymInfo)
  : Relocation(pType, pTargetData, pTargetRef, (DWord)pAddend, pSymInfo) {
}


ARMRelocation::~ARMRelocation()
{
}


