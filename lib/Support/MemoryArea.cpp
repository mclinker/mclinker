/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/RegionFactory.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/FileSystem.h>
#include <llvm/Support/ErrorHandling.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cerrno>
#include <sstream>

using namespace mcld;

//===--------------------------------------------------------------------===//
// MemoryArea
MemoryArea::MemoryArea(RegionFactory& pRegionFactory, const sys::fs::Path& pPath)
  : m_RegionFactory(pRegionFactory), m_FilePath(pPath), m_FileDescriptor(-1) {
}

MemoryArea::~MemoryArea()
{
  if (isGood())
    close();
}

void MemoryArea::open(const sys::fs::Path& pPath, int pFlags)
{
  m_FileDescriptor = ::open(pPath.c_str(), pFlags);
}

void MemoryArea::open(const sys::fs::Path& pPath, int pFlags, int pMode)
{
  m_FileDescriptor = ::open(pPath.c_str(), pFlags, pMode);
}

void MemoryArea::close()
{
  ::close(m_FileDescriptor);
  m_FileDescriptor = -1;  
}

bool MemoryArea::isGood() const
{
  return (-1 != m_FileDescriptor);
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
  Space* space = find(pOffset, pLength);
  MemoryArea::Address r_start = 0;
  if (0 == space) { // not found
    space = new Space();
    space->file_offset = pOffset;
    m_SpaceList.push_back(space);
    switch(space->type = policy(pOffset, pLength)) {
      case Space::MMAPED: {
        // FIXME: implement memory mapped I/O 
        // compute correct r_start, space.data, and space.size
        // space.data and space.size should be on page boundaries.
        break;
      }
      case Space::ALLOCATED_ARRAY: {
        space->size = pLength;
        space->data = new unsigned char[pLength];
        size_t read_bytes = sys::fs::detail::pread(m_FileDescriptor,
                                          space->data,
                                          space->size,
                                          space->file_offset);
        if (read_bytes == pLength) {
          r_start = space->data;
          break;
        }
        else {
          std::stringstream error_mesg;
          error_mesg << m_FilePath.native();
          if (read_bytes < 0) {
            error_mesg << ":pread failed: ";
            error_mesg << sys::fs::detail::strerror(errno) << '\n';
          }
          else if (read_bytes < pLength) {
            error_mesg << ": file too short: read only ";
            error_mesg << read_bytes << " of " << space->size << " bytes at ";
            error_mesg << space->file_offset << std::endl;
          }
          else {
            error_mesg << ": implementation of detail::pread reads exceeding bytes.\n";
            error_mesg << "pread( " << m_FilePath.native() << ", buf, "
                       << space->size << ", " << space->file_offset << '\n';
          }
          llvm::report_fatal_error(error_mesg.str());
        }
      } // case
    } // switch
  }
  else { // found
    off_t distance = pOffset - space->file_offset;
    r_start = space->data + distance;
  }
  // now, we have a legal space to hold the new MemoryRegion
  MemoryRegion* result = m_RegionFactory.produce(r_start, pLength);
  return result;
}

// release - release a MemoryRegion
void MemoryArea::release(MemoryRegion* pRegion)
{
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
  // FIXME: implement memory mapped I/O
//  const size_t threshold = 3072; // 3/4 page size in Linux
//  if (pLength < threshold)
    return Space::ALLOCATED_ARRAY;
//  else
//    return Space::MMAPED;
}

