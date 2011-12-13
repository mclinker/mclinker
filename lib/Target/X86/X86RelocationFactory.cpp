//===- X86RelocationFactory.cpp -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

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

void X86RelocationFactory::apply(Relocation& pRelocation)
{
}

void X86RelocationFactory::destroy(Relocation* pRelocation)
{
}
