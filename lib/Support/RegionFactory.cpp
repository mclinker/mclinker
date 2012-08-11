//===- RegionFactory.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/RegionFactory.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/Space.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// RegionFactory
//===----------------------------------------------------------------------===//
RegionFactory::RegionFactory()
{
}

RegionFactory::~RegionFactory()
{
}

MemoryRegion* RegionFactory::produce(Space& pSpace, void* pVMAStart, size_t pSize)
{
  MemoryRegion* result = Alloc::allocate();
  new (result) MemoryRegion(pSpace,
                            static_cast<const MemoryRegion::Address>(pVMAStart),
                            pSize);
  pSpace.addRegion(*result);
  return result;
}

void RegionFactory::destruct(MemoryRegion* pRegion)
{
  pRegion->parent()->removeRegion(*pRegion);
  destroy(pRegion);
  deallocate(pRegion);
}

