//===- MemoryArea.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MEMORY_AREA_H
#define MCLD_MEMORY_AREA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/Uncopyable.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/Space.h>
#include <fcntl.h>
#include <string>
#include <list>


#if defined(ENABLE_UNITTEST)
namespace mcldtest
{
  class MemoryAreaTest;
} // namespace of mcldtest
#endif

namespace mcld
{

class MemoryRegion;
class RegionFactory;

/** \class MemoryArea
 *  \brief MemoryArea is used to manage distinct MemoryRegions of address space.
 *
 *  Good linkers must well manipulate memory mapped I/O and dynamic memory.
 *  In MCLinker, MemoryArea is the decision-maker to use memory mapped I/O or
 *  dynamic memory. When a client requests MemoryArea for a piece of memory
 *  to hold a part of a file, MemoryArea is going to see whether the requested
 *  part of the file is already in any existing memory which is requested
 *  before. If it is, MemoryArea creates a new MemoryRegion within the memory
 *  requested before. Otherwise, MemoryArea uses memory mapped I/O or dynamic
 *  memory to load the file.
 *
 *  If the part a file being loaded is larger than 3/4 pages, MemoryArea uses
 *  memory mapped I/O to load the file. Otherwise, MemoryArea uses dynamic
 *  memory to read the content of file into the memory space.
 */
class MemoryArea : private Uncopyable
{
  friend class MemoryAreaFactory;
public:
  typedef llvm::iplist<Space> SpaceList;

public:
  // constructor by file handler.
  // If the given file handler is read-only, client can not request a region
  // that out of the file size.
  // @param pFileHandle - file handler
  MemoryArea(RegionFactory& pRegionFactory, FileHandle& pFileHandle);

  // constructor by set universal space.
  // Client can not request a region that out of the universal space.
  // @param pUniverse - file handler
  MemoryArea(RegionFactory& pRegionFactory, Space& pUniverse);

  // destructor
  ~MemoryArea();

  // request - create a MemoryRegion within a sufficient space
  // find an existing space to hold the MemoryRegion.
  // if MemoryArea does not find such space, then it creates a new space and
  // assign a MemoryRegion into the space.
  MemoryRegion* request(size_t pOffset, size_t pLength);

  // release - release a MemoryRegion.
  // release a MemoryRegion does not cause
  void release(MemoryRegion* pRegion);

  // clear - release all memory regions.
  void clear();

  FileHandle* handler()
  { return m_pFileHandle; }

  const FileHandle* handler() const
  { return m_pFileHandle; }

  // -----  space list methods  ----- //
  Space* find(size_t pOffset, size_t pLength);

  const Space* find(size_t pOffset, size_t pLength) const;

private:
  RegionFactory& m_RegionFactory;
  SpaceList m_SpaceList;
  FileHandle* m_pFileHandle;
};

} // namespace of mcld

#endif

