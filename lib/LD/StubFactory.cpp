//===- StubFactory.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/StubFactory.h>
#include <mcld/IRBuilder.h>
#include <mcld/LD/BranchIslandFactory.h>
#include <mcld/LD/BranchIsland.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/Fragment/Stub.h>
#include <mcld/Fragment/Relocation.h>

#include <string>

using namespace mcld;

//===----------------------------------------------------------------------===//
// StubFactory
//===----------------------------------------------------------------------===//
StubFactory::~StubFactory()
{
  for (StubPoolType::iterator it = m_StubPool.begin(), ie = m_StubPool.end();
       it != ie; ++it)
    delete(*it);
}

/// addPrototype - register a stub prototype
void StubFactory::addPrototype(Stub* pPrototype)
{
  m_StubPool.push_back(pPrototype);
}

/// create - create a stub if needed, otherwise return NULL
Stub* StubFactory::create(Relocation& pReloc,
                          uint64_t pTargetSymValue,
                          IRBuilder& pBuilder,
                          BranchIslandFactory& pBRIslandFactory)
{
  // find if there is a prototype stub for the input relocation
  Stub* stub = NULL;
  Stub* prototype = findPrototype(pReloc, pReloc.place(), pTargetSymValue);
  if (prototype != NULL) {
    const Fragment* frag = pReloc.targetRef().frag();
    // find the islands for the input relocation
    std::pair<BranchIsland*, BranchIsland*> islands =
        pBRIslandFactory.getIslands(*frag);
    if (islands.first == NULL) {
      // early exit if we can not find the forward island.
      return NULL;
    }

    // find if there is such a stub in the backward island first.
    if (islands.second != NULL) {
      stub = islands.second->findStub(prototype, pReloc);
    }

    if (stub != NULL) {
      // reset the branch target to the stub instead!
      pReloc.setSymInfo(stub->symInfo());
    } else {
      // find if there is such a stub in the forward island.
      stub = islands.first->findStub(prototype, pReloc);
      if (stub != NULL) {
        // reset the branch target to the stub instead!
        pReloc.setSymInfo(stub->symInfo());
      } else {
        // create a stub from the prototype
        stub = prototype->clone();

        // build a name for stub symbol
        std::string name("__");
        name.append(pReloc.symInfo()->name())
            .append("_")
            .append(stub->name())
            .append("@")
            .append(islands.first->name());

        // create LDSymbol for the stub
        LDSymbol* symbol =
            pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Unresolve>(
                name,
                ResolveInfo::Function,
                ResolveInfo::Define,
                ResolveInfo::Local,
                stub->size(), // size
                stub->initSymValue(), // value
                FragmentRef::Create(*stub, stub->initSymValue()),
                ResolveInfo::Default);
        stub->setSymInfo(symbol->resolveInfo());

        // add relocations of this stub (i.e., set the branch target of the stub)
        for (Stub::fixup_iterator it = stub->fixup_begin(),
             ie = stub->fixup_end(); it != ie; ++it) {

          Relocation* reloc =
              Relocation::Create((*it)->type(),
                                 *(FragmentRef::Create(*stub, (*it)->offset())),
                                 (*it)->addend());
          reloc->setSymInfo(pReloc.symInfo());
          islands.first->addRelocation(*reloc);
        }

        // add stub to the forward branch island
        islands.first->addStub(prototype, pReloc, *stub);

        // reset the branch target of the input reloc to this stub instead!
        pReloc.setSymInfo(stub->symInfo());
      }
    }
  }
  return stub;
}

/// findPrototype - find if there is a registered stub prototype for the given
/// relocation
Stub* StubFactory::findPrototype(const Relocation& pReloc,
                                 uint64_t pSource,
                                 uint64_t pTargetSymValue)
{
  for (StubPoolType::iterator it = m_StubPool.begin(), ie = m_StubPool.end();
       it != ie; ++it) {
    if ((*it)->isMyDuty(pReloc, pSource, pTargetSymValue))
      return (*it);
  }
  return NULL;
}
