/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#include <mcld/LD/RelocationFactory.h>

using namespace mcld;

//==========================
// RelocationFactory
RelocationFactory::RelocationFactory(size_t pNum)
  : GCFactory<Relocation, 0>(pNum) {
}

RelocationFactory::~RelocationFactory()
{
}

Relocation* RelocationFactory::produce(Type pType,
                                       const LDSymbol& pSymbol,
                                       Address pOffset,
                                       Address pAddend)

{
}

void RelocationFactory::destroy(Relocation* pRelocation)
{
}

