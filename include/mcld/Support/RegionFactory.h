/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_REGION_FACTORY_H
#define MCLD_REGION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/FileSystem.h>

namespace mcld
{

class MemoryArea;

/** \class RegionFactory
 *  \brief RegionFactory produces and destroys MemoryRegions
 *
 */
class RegionFactory : public GCFactory<MemoryRegion, 0>
{
public:
  typedef GCFactory<MemoryRegion, 0> Alloc;

public:
  RegionFactory(size_t pNum);
  ~RegionFactory();

  // ----- production ----- //
  MemoryRegion* produce(MemoryArea::Space* pSpace,
                        const sys::fs::detail::Address pVMAStart,
                        size_t pSize);
};

} // namespace of mcld

#endif

