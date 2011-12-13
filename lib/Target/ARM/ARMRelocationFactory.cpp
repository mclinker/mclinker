//===- ARMRelocationFactory.cpp  ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

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

void ARMRelocationFactory::apply(Relocation& Relocation)
{
}

void ARMRelocationFactory::destroy(Relocation* pRelocation)
{
  /** GCFactory will recycle the relocation **/
}

