//===- StaticResolver.cpp -------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/StaticResolver.h"
#include "mcld/LD/LDSymbol.h"
#include <cassert>

using namespace std;
using namespace mcld;

//==========================
// StaticResolver
StaticResolver::StaticResolver()
{
}

StaticResolver::~StaticResolver()
{
}

unsigned int StaticResolver::resolve(const ResolveInfo & __restrict__ pOld,
                                     const ResolveInfo & __restrict__ pNew,
                                     bool &pOverride)
{
}


