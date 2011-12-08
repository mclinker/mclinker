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

Relocation* RelocationFactory::produce(RelocationFactory::Type pType,
                                       MCFragmentRef& pFragRef,
				       RelocationFactory::DWord pTarget,
                                       RelocationFactory::Address pAddend)

{
  Relocation* result = GCFactory<Relocation, 0>::allocate();
  new (result) Relocation(pType, pFragRef, pAddend, pTarget, *this);
  return result;
}

RelocationFactory::~RelocationFactory()
{
}

