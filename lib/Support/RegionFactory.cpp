//===- RegionFactory.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/RegionFactory.h>
#include <mcld/Support/Space.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// RegionFactory
//===----------------------------------------------------------------------===//
MemoryRegion*
RegionFactory::produce(Address pVMAStart, size_t pSize, Space& pSpace)
{
  MemoryRegion* result = Alloc::allocate();
  new (result) MemoryRegion(pSpace, pVMAStart, pSize);
  return result;
}

void RegionFactory::destruct(MemoryRegion* pRegion)
{
  destroy(pRegion);
  deallocate(pRegion);
}

