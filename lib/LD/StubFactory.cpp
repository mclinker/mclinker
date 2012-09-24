//===- StubFactory.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/StubFactory.h>
#include <mcld/Fragment/Stub.h>

#include <string>
using namespace mcld;

StubFactory::StubFactory()
{
}

StubFactory::~StubFactory()
{
  for (StubPoolType::iterator it = m_StubPool.begin(), ie = m_StubPool.end();
       it != ie; ++it)
    delete(*it);
}

void StubFactory::addPrototype(Stub* pPrototype)
{
  m_StubPool.push_back(pPrototype);
}

Stub* StubFactory::create(Relocation& pReloc,
                          uint64_t pTargetSymValue,
                          FragmentLinker& pLinker,
                          BranchIslandFactory& pBRIslandFactory)
{
  return NULL;
}

Stub* StubFactory::findPrototype(const Relocation& pReloc,
                                 uint64_t pSource,
                                 uint64_t pTargetSymValue)
{
  return NULL;
}

