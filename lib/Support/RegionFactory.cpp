//===- RegionFactory.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/RegionFactory.h"
#include "mcld/Support/MemoryArea.h"

using namespace mcld;

//==========================
// RegionFactory
RegionFactory::RegionFactory(size_t pNum)
  : GCFactory<MemoryRegion, 0>(pNum) {
}

RegionFactory::~RegionFactory()
{
}

MemoryRegion* RegionFactory::produce(MemoryArea::Space* pSpace,
                                     const sys::fs::detail::Address pVMAStart,
                                     size_t pSize)
{
  MemoryRegion* result = Alloc::allocate();
  new (result) MemoryRegion(pSpace, pVMAStart, pSize);
  return result;
}

void RegionFactory::destruct(MemoryRegion* pRegion)
{
  pRegion->drift();
  destroy(pRegion);
  deallocate(pRegion);
}

