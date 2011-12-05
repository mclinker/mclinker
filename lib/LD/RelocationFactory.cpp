/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/Relocation.h>

using namespace mcld;

//==========================
// RelocationFactory
RelocationFactory::RelocationFactory(size_t pNum)
  : GCFactory<Relocation, 0>(pNum) {
}

RelocationFactory::~RelocationFactory()
{
}

const Howto* RelocationFactory::typeToHowto(RelocationFactory::Type pRelocType) const
{
   return 0;
}

Relocation* RelocationFactory::produce(RelocationFactory::Type pType,
                                       const LDSymbol& pSymbol,
                                       MCFragmentRef& pFragRef,
                                       RelocationFactory::Address pAddend)

{
  Relocation* result = GCFactory<Relocation, 0>::allocate();
  const Howto *howto = RelocationFactory::typeToHowto(pType);
  const ResolveInfo *info = pSymbol.resolveInfo();
  new (result) Relocation(*howto, pFragRef, pAddend, *info);
  return result;
}

void RelocationFactory::destroy(Relocation* pRelocation)
{
}

