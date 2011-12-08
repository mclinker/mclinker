/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include "X86RelocationFactory.h"

using namespace mcld;

//==========================
// X86RelocationFactory

X86RelocationFactory::X86RelocationFactory(size_t pNum) : 
                                           RelocationFactory(pNum)
{
}

X86RelocationFactory::~X86RelocationFactory()
{
}

const RelocationFactory::Pointer X86RelocationFactory::getApply(
                                 RelocationFactory::Type pRelType) const
{
  return 0; //TODO:
}

void X86RelocationFactory::destroy(Relocation* pRelocation)
{
}
