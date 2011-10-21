//===- MemoryArea.cpp -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/RegionFactory.h"
#include "mcld/Support/MemoryArea.h"
#include "mcld/Support/MemoryRegion.h"
#include "mcld/Support/FileSystem.h"
#include "llvm/Support/ErrorHandling.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <cerrno>
#include <sstream>

using namespace mcld;

#include <iostream>
using namespace std;
//===--------------------------------------------------------------------===//
// MemoryArea
MemoryArea::MemoryArea(RegionFactory& pRegionFactory)
  : m_RegionFactory(pRegionFactory),
    m_FileDescriptor(-1),
    m_AccessFlags(0),
    m_State(BadBit) {
}

MemoryArea::~MemoryArea()
{
  unmap();
}

void MemoryArea::map(const sys::fs::Path& pPath, int pFlags)
{
  m_AccessFlags = pFlags;
  m_FilePath = pPath;
  m_FileDescriptor = ::open(m_FilePath.c_str(), m_AccessFlags);
  if (-1 == m_FileDescriptor)
    m_State |= FailBit;
  else
    m_State = GoodBit;
}

void MemoryArea::map(const sys::fs::Path& pPath, int pFlags, int pMode)
{
  m_AccessFlags = pFlags;
  m_FilePath = pPath;
  m_FileDescriptor = ::open(m_FilePath.c_str(), m_AccessFlags, pMode);
  if (-1 == m_FileDescriptor)
    m_State |= FailBit;
  else
    m_State = GoodBit;
}

void MemoryArea::unmap()
{
  if (isMapped()) {
    if (-1 == ::close(m_FileDescriptor))
      m_State |= FailBit;
    else {
      m_FileDescriptor = -1;
      m_AccessFlags = 0;
    }
  }
}

bool MemoryArea::isMapped() const
{
  return (-1 != m_FileDescriptor);
}

bool MemoryArea::isGood() const
{
  return !(m_State & (BadBit | FailBit));
}

bool MemoryArea::isBad() const
{
  return (m_State & BadBit);
}

bool MemoryArea::isFailed() const
{
  return (m_State & FailBit);
}

bool MemoryArea::isEOF() const
{
  return (m_State & EOFBit);
}

int MemoryArea::rdstate() const
{
  return m_State;
}

void MemoryArea::setState(MemoryArea::IOState pState)
{
  m_State |= pState;
}

void MemoryArea::clear(MemoryArea::IOState pState)
{
  m_State = pState;
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
// space.file_offset is the offset of the mapped file segment from the start of the file.
// if the MemorySpace's type is ALLOCATED_ARRAY, the distances of (space.data, r_start)
// and (r_len, space.size) are zero.
//
MemoryRegion* MemoryArea::request(off_t pOffset, size_t pLength)
{
  if (!isMapped() || !isGood())
    return 0;
  Space* space = find(pOffset, pLength);
  MemoryArea::Address r_start = 0;
  if (0 == space) { // not found, first reach the memory space
    space = new Space(this, pOffset, pLength);
    m_SpaceList.push_back(space);
    switch(space->type = policy(pOffset, pLength)) {
      case Space::MMAPED: {
        // compute correct r_start, space.data, and space.size
        // space.data and space.size should be on page boundaries.
        space->file_offset = page_boundary(pOffset);
        space->size = page_offset(pOffset+pLength) - space->file_offset;
        space->data = (Address) ::mmap(NULL,
                             space->size,
                             PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED,
                             m_FileDescriptor,
                             space->file_offset);
        r_start = space->data + (pOffset - space->file_offset);
        break;
      }
      case Space::ALLOCATED_ARRAY: {
        space->data = new unsigned char[pLength];
	r_start = space->data;
        if (WriteOnly & m_AccessFlags)
          break;
        else { // read-only or read-write
          size_t read_bytes = sys::fs::detail::pread(m_FileDescriptor,
                                                     space->data,
                                                     space->size,
                                                     space->file_offset);
          if (read_bytes == pLength)
            break;
          else {
            std::stringstream error_mesg;
            error_mesg << m_FilePath.native();
            if (read_bytes < 0) {
              m_State |= FailBit;
              error_mesg << ":pread failed: ";
              error_mesg << sys::fs::detail::strerror(errno) << '\n';
            }
            else if (read_bytes < pLength) {
              m_State |= EOFBit;
              if (ReadWrite & m_AccessFlags) // read-write permission allows EOF while pread
                break;
              m_State |= BadBit;
              error_mesg << ": file too short: read only ";
              error_mesg << read_bytes << " of " << space->size << " bytes at ";
              error_mesg << space->file_offset << std::endl;
            }
            else {
              m_State |= BadBit;
              error_mesg << ": implementation of detail::pread reads exceeding bytes.\n";
              error_mesg << "pread( " << m_FilePath.native() << ", buf, "
                         << space->size << ", " << space->file_offset << '\n';
            }
            llvm::report_fatal_error(error_mesg.str());
          }
        }
      } // case
    } // switch
  }
  else { // found
    off_t distance = pOffset - space->file_offset;
    r_start = space->data + distance;
  }
  // now, we have a legal space to hold the new MemoryRegion
  MemoryRegion* result = m_RegionFactory.produce(space, r_start, pLength);
  return result;
}

// release - release a MemoryRegion
void MemoryArea::release(MemoryRegion* pRegion)
{
  if (!isMapped() || !isGood())
    return;
  pRegion->sync();
  m_RegionFactory.destroy(pRegion);
  m_RegionFactory.deallocate(pRegion);
}

void MemoryArea::clean()
{
  m_RegionFactory.clear();
  
  SpaceList::iterator sIter, sEnd = m_SpaceList.end();
  for (sIter = m_SpaceList.begin(); sIter!=sEnd; ++sIter) {
    release(sIter);
  }
  m_SpaceList.clear();
}

MemoryArea::Space* MemoryArea::find(off_t pOffset, size_t pLength)
{
  SpaceList::iterator sIter, sEnd = m_SpaceList.end();
  for (sIter = m_SpaceList.begin(); sIter!=sEnd; ++sIter) {
    if (sIter->file_offset <= pOffset && (pOffset+static_cast<off_t>(pLength))
       <= (sIter->file_offset+static_cast<off_t>(sIter->size)) ) { // within
      return sIter;
    }
  }
  return 0;
}

void MemoryArea::release(MemoryArea::Space* pSpace)
{
    write(*pSpace);
    switch (pSpace->type) {
      case Space::ALLOCATED_ARRAY: {
        delete [] pSpace->data;
        break;
      }
      case Space::MMAPED: {
        ::munmap(pSpace->data, pSpace->size);
        break;
      }
    }
    delete pSpace;
}

MemoryArea::Space::Type MemoryArea::policy(off_t pOffset, size_t pLength)
{
  const size_t threshold = (PageSize*3)/4; // 3/4 page size in Linux
  if (pLength < threshold)
    return Space::ALLOCATED_ARRAY;
  else
    return Space::MMAPED;
}

void MemoryArea::write(const Space& pSpace)
{
  if (!isMapped() || !isGood())
    return;

  switch(pSpace.type) {
    case Space::MMAPED: {
      if(-1 == ::msync(pSpace.data, pSpace.size, MS_SYNC))
        m_State |= FailBit;
      return;
    }
    case Space::ALLOCATED_ARRAY: {
      int write_bytes = sys::fs::detail::pwrite(m_FileDescriptor,
                                                pSpace.data,
                                                pSpace.size,
                                                pSpace.file_offset);
      if (0 > write_bytes) {
        m_State |= FailBit;
        return;
      }
      if (0 == write_bytes && 0 != pSpace.size)
        m_State |= BadBit;
      if ( pSpace.size > write_bytes )
        m_State |= EOFBit;
      return;
    }
    default: {
    }
  }
}

