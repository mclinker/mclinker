//===- MemoryArea.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/RegionFactory.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MsgHandling.h>

using namespace mcld;

//===--------------------------------------------------------------------===//
// MemoryArea
//===--------------------------------------------------------------------===//

// MemoryArea - special constructor
// This constructor is used for *SPECIAL* situation. I'm sorry I can not
// reveal what is the special situation.
MemoryArea::MemoryArea(RegionFactory& pRegionFactory, Space& pUniverse)
  : m_RegionFactory(pRegionFactory), m_pFileHandle(NULL) {
  m_SpaceList.push_back(&pUniverse);
}

MemoryArea::MemoryArea(RegionFactory& pRegionFactory, FileHandle& pFileHandle)
  : m_RegionFactory(pRegionFactory), m_pFileHandle(&pFileHandle) {
}

MemoryArea::~MemoryArea()
{
}

// The layout of MemorySpace in the virtual memory space
//
// |  : page boundary
// [,]: MemoryRegion
// -  : fillment
// =  : data
//
// |---[=|====|====|==]--|
// ^   ^              ^  ^
// |   |              |  |
// | r_start      +r_len |
// space.data      +space.size
//
// space.file_offset is the offset of the mapped file segment from the start of
// the file. if the MemorySpace's type is ALLOCATED_ARRAY, the distances of
// (space.data, r_start) and (r_len, space.size) are zero.
//
MemoryRegion* MemoryArea::request(size_t pOffset, size_t pLength)
{
  Space* space = find(pOffset, pLength);
  if (NULL == space) {
    // not found
    if (NULL == m_pFileHandle) {
      // if m_pFileHandle is NULL, clients delegate us an universal Space and
      // we never remove it. In that way, space can not be NULL.
      unreachable(diag::err_out_of_range_region) << pOffset << pLength;
    }

    space = Space::createSpace(*m_pFileHandle, pOffset, pLength);
    m_SpaceList.push_back(space);
  }

  // adjust r_start
  off_t distance = pOffset - space->start();
  void* r_start = space->memory() + distance;

  // now, we have a legal space to hold the new MemoryRegion
  return m_RegionFactory.produce(*space, r_start, pLength);
}

// release - release a MemoryRegion
void MemoryArea::release(MemoryRegion* pRegion)
{
  if (NULL == pRegion)
    return;

  Space *space = pRegion->parent();
  m_RegionFactory.destruct(pRegion);

  if (0 == space->numOfRegions()) {

    if (NULL != m_pFileHandle) {
      // if m_pFileHandle is NULL, clients delegate us an universal Space and
      // we never remove it. Otherwise, we have to synchronize and release
      // Space.
      if (m_pFileHandle->isWritable()) {
        // synchronize writable space before we release it.
        Space::syncSpace(space, *m_pFileHandle);
      }
      Space::releaseSpace(space, *m_pFileHandle);
      m_SpaceList.erase(space);
    }
  }
}

// clear - release all MemoryRegions
void MemoryArea::clear()
{
  if (NULL == m_pFileHandle)
    return;

  if (m_pFileHandle->isWritable()) {
    SpaceList::iterator space, sEnd = m_SpaceList.end();
    for (space = m_SpaceList.begin(); space != sEnd; ++space) {
      Space::syncSpace(space, *m_pFileHandle);
      Space::releaseSpace(space, *m_pFileHandle);
    }
  }
  else {
    SpaceList::iterator space, sEnd = m_SpaceList.end();
    for (space = m_SpaceList.begin(); space != sEnd; ++space)
      Space::releaseSpace(space, *m_pFileHandle);
  }

  m_SpaceList.clear();
}

//===--------------------------------------------------------------------===//
// SpaceList methods
Space* MemoryArea::find(size_t pOffset, size_t pLength)
{
  SpaceList::iterator sIter, sEnd = m_SpaceList.end();
  for (sIter = m_SpaceList.begin(); sIter!=sEnd; ++sIter) {
    if (sIter->start() <= pOffset &&
       (pOffset+pLength) <= (sIter->start()+sIter->size()) ) {
      // within
      return sIter;
    }
  }
  return NULL;
}

const Space* MemoryArea::find(size_t pOffset, size_t pLength) const
{
  SpaceList::const_iterator sIter, sEnd = m_SpaceList.end();
  for (sIter = m_SpaceList.begin(); sIter!=sEnd; ++sIter) {
    if (sIter->start() <= pOffset &&
       (pOffset+pLength) <= (sIter->start()+sIter->size()) ) {
      // within
      return sIter;
    }
  }
  return NULL;
}

