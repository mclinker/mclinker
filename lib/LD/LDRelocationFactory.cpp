/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#include <mcld/LD/LDRelocationFactory.h>

using namespace mcld;

//==========================
// LDRelocationFactory
LDRelocationFactory::LDRelocationFactory(size_t pNum)
  : GCFactory<LDRelocation, 0>(pNum) {
}

LDRelocationFactory::~LDRelocationFactory()
{
}

LDRelocation* LDRelocationFactory::produce(const LDHowto& pHowto, 
	                              MCFragmentRef& pFragmentRef, 
	                              uint64_t pAddend,
		                          uint32_t pType)
{
  LDRelocation* result = Alloc::allocate();
  new (result) LDRelocation(pHowto, pFragmentRef, pAddend, pType) ;
  return result;  
}