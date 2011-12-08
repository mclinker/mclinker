/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include "ARMRelocationFactory.h"
//TODO: Manually include all relocations

using namespace mcld;

//==========================
// ARMRelocationFactory
ARMRelocationFactory::ARMRelocationFactory(size_t pNum) : RelocationFactory(pNum)
{
}

ARMRelocationFactory::~ARMRelocationFactory()
{
}


const RelocationFactory::Pointer ARMRelocationFactory::getApply(
                                 RelocationFactory::Type pRelType) const
{
   return 0; //TODO
}
void ARMRelocationFactory::destroy(Relocation* pRelocation)
{
  /** GCFactory will recycle the relocation **/
}

