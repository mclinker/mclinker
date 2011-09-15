/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/RegionFactory.h>

using namespace mcld;

//==========================
// RegionFactory
RegionFactory::RegionFactory(size_t pNum)
  : GCFactory<MemoryRegion, 0>(pNum) {
}

RegionFactory::~RegionFactory()
{
}

MemoryRegion* RegionFactory::produce(const sys::fs::detail::Address pVMAStart,
		size_t pSize)
{
  MemoryRegion* result = Alloc::allocate();
  new (result) MemoryRegion(pVMAStart, pSize);
  return result;
}

