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
ARMRelocationFactory::ARMRelocationFactory(size_t pNum)
  : GCFactory<Relocation, 0>(pNum) {
}

ARMRelocationFactory::~ARMRelocationFactory()
{
}

Relocation* ARMRelocationFactory::produce(Type pType,
                                          const LDSymbol& pSymbol,
                                          MCFragmentRef& pFragRef,
                                          DWord* pTargetData,
                                          Address pAddend)
{
/** list all relocation
  static Relocations[] = {
    
  };

  // clone a relocation
  // set up the relocation
  // return the relocation
**/
  return NULL;
}

void ARMRelocationFactory::destroy(Relocation* pRelocation)
{
  /** GCFactory will recycle the relocation **/
}

