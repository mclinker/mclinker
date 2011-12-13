//===- MemoryAreaFactory.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MEMORY_AREA_FACTORY_H
#define MCLD_MEMORY_AREA_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/Support/UniqueGCFactory.h"
#include "mcld/Support/MemoryArea.h"
#include "mcld/Support/Path.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

namespace mcld
{

class RegionFactory;
/** \class MemoryAreaFactory
 *  \brief MemoryAreaFactory avoids creating duplicated MemoryAreas of the
 *   same file.
 *
 *  Users can give duplicated input files on the command line. In order to 
 *  prevent opening the same file twice, and create redundant MemoryRegions,
 *  mcld::Input should not create MemoryArea directly. Instead, it should ask
 *  MemoryAreaFactory and get the unique MemoryArea.
 *
 *  The timing of opening and closing files is not strictly bound to the
 *  constructor and destructor of MCLDFile. For mcld::Output, MCLinker
 *  opens the file rather after assigning offset to sections. On the other
 *  aside, mcld::Input opens the file at constructor. In order to hide the
 *  file operations, MemoryAreaFactory actually open the file untill the first
 *  MemoryRegion is requested.
 *
 *  @see MemoryRegion
 *  @see UniqueGCFactoryBase
 */
class MemoryAreaFactory : public UniqueGCFactoryBase<sys::fs::Path, MemoryArea, 0>
{
public:
  explicit MemoryAreaFactory(size_t pNum);
  ~MemoryAreaFactory();

  // produce - create a MemoryArea and open its file
  // If the file fails to be opened, the returned MemoryArea::isMapped() 
  // should be false
  MemoryArea* produce(const sys::fs::Path& pPath, int pFlags);
  MemoryArea* produce(const sys::fs::Path& pPath, int pFlags, mode_t pMode);

private:
  RegionFactory* m_pRegionFactory;
};

} // namespace of mcld

#endif

