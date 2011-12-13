//===- RelocationFactory.cpp ----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/RelocationFactory.h"
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

void RelocationFactory::destroy(Relocation* pRelocation)
{
   /** GCFactory will recycle the relocation **/
}

