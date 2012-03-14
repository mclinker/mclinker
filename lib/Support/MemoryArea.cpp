//===- MemoryArea.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/ErrorHandling.h>
#include <llvm/ADT/Twine.h>

#include <mcld/Support/RegionFactory.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/FileSystem.h>

#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

using namespace mcld;

//===--------------------------------------------------------------------===//
// MemoryArea
MemoryArea::MemoryArea(RegionFactory& pRegionFactory)
  : m_RegionFactory(pRegionFactory),
    m_FileDescriptor(-1),
    m_FileSize(0),
    m_AccessFlags(ReadOnly),
    m_State(BadBit) {
}

MemoryArea::~MemoryArea()
{
  // truncate the file to real size
  if (isWritable())
    truncate(m_FileSize);

  unmap();
}

void MemoryArea::truncate(size_t pLength)
{
  if (!isWritable())
    return;

  if (-1 == ::ftruncate(m_FileDescriptor, static_cast<off_t>(pLength))) {
    llvm::report_fatal_error(llvm::Twine("Cannot truncate `") +
                             m_FilePath.native() +
                             llvm::Twine("' to size: ") +
                             llvm::Twine(pLength) +
                             llvm::Twine(".\n"));
  }
}

void MemoryArea::map(const sys::fs::Path& pPath, int pFlags)
{
  m_AccessFlags = pFlags;
  m_FilePath = pPath;
  m_FileDescriptor = ::open(m_FilePath.c_str(), m_AccessFlags);

  if (-1 == m_FileDescriptor) {
    m_State |= FailBit;
  }
  else {
    struct stat st;
    int stat_result = ::stat(m_FilePath.native().c_str(), &st);
    if (0x0 == stat_result) {
      m_FileSize = static_cast<size_t>(st.st_size);
      m_State = GoodBit;
    }
    else {
      m_FileSize = 0x0;
      m_State |= FailBit;
      m_State |= BadBit;
    }
  }
}

void MemoryArea::map(const sys::fs::Path& pPath, int pFlags, int pMode)
{
  m_AccessFlags = pFlags;
  m_FilePath = pPath;
  m_FileDescriptor = ::open(m_FilePath.c_str(), m_AccessFlags, pMode);

  if (-1 == m_FileDescriptor) {
    m_State |= FailBit;
  }
  else {
    struct stat st;
    int stat_result = ::stat(m_FilePath.native().c_str(), &st);
    if (0x0 == stat_result) {
      m_FileSize = static_cast<size_t>(st.st_size);
      m_State = GoodBit;
    }
    else {
      m_FileSize = 0x0;
      m_State |= FailBit;
      m_State |= BadBit;
    }
  }
}

void MemoryArea::unmap()
{
  if (isMapped()) {
    if (-1 == ::close(m_FileDescriptor))
      m_State |= FailBit;
    else {
      m_FileDescriptor = -1;
      m_AccessFlags = ReadOnly;
    }
  }
}

bool MemoryArea::isMapped() const
{
  return (-1 != m_FileDescriptor);
}

bool MemoryArea::isGood() const
{
  return 0x0 == (m_State & (BadBit | FailBit));
}

bool MemoryArea::isBad() const
{
  return 0x0 != (m_State & BadBit);
}

bool MemoryArea::isFailed() const
{
  return 0x0 != (m_State & FailBit);
}

bool MemoryArea::isEOF() const
{
  return 0x0 != (m_State & EOFBit);
}

bool MemoryArea::isReadable() const
{
  return (((m_AccessFlags & AccessMask) == ReadOnly) ||
         ((m_AccessFlags & AccessMask) == ReadWrite));
}

bool MemoryArea::isWritable() const
{
  return (((m_AccessFlags & AccessMask) == WriteOnly) ||
         ((m_AccessFlags & AccessMask) == ReadWrite));
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
// space.file_offset is the offset of the mapped file segment from the start of
// the file. if the MemorySpace's type is ALLOCATED_ARRAY, the distances of
// (space.data, r_start) and (r_len, space.size) are zero.
//
MemoryRegion* MemoryArea::request(size_t pOffset, size_t pLength)
{
  if (!isMapped() || !isGood())
    return NULL;

  if (0x0 == pLength)
    return NULL;

  if (!isWritable() && (pOffset + pLength) > m_FileSize)
    return NULL;

  if (isWritable() && (pOffset + pLength) > m_FileSize) {
    // If the memory area is writable, user can expand the size of file by
    // request a region larger than the file.
    // MemoryArea should enlarge the file if the requested region is larger
    // than the file.
    m_FileSize = page_boundary(pOffset + pLength + 1);
    truncate(m_FileSize);
  }

  Space* space = find(pOffset, pLength);
  MemoryArea::Address r_start = 0;
  if (NULL == space) {
    // the space does not exist, create a new space.
    space = new Space(this, pOffset, pLength);
    m_SpaceList.push_back(space);
    switch(space->type = policy(pOffset, pLength)) {
      case Space::MMAPED: {
        int mm_prot, mm_flag;
        if (isWritable()) {
          mm_prot = PROT_READ | PROT_WRITE;
          mm_flag = MAP_FILE | MAP_SHARED;
        }
        else {
          mm_prot = PROT_READ;
          mm_flag = MAP_FILE | MAP_PRIVATE;
        }

        space->file_offset = page_offset(pOffset);

        // The space's size may be larger than filesize.
        space->size = page_boundary(pLength + pOffset + 1 - space->file_offset);
        space->data = (Address) ::mmap(NULL,
                                       space->size,
                                       mm_prot, mm_flag,
                                       m_FileDescriptor,
                                       space->file_offset);

        if (space->data == MAP_FAILED) {
          llvm::report_fatal_error(llvm::Twine("cannot open memory map file :") +
                                   m_FilePath.native() +
                                   llvm::Twine(" (") +
                                   sys::fs::detail::strerror(errno) +
                                   llvm::Twine(").\n"));
        }

        r_start = space->data + (pOffset - space->file_offset);
        break;
      }
      case Space::ALLOCATED_ARRAY: {
        // space->offset and space->size are set in constructor. We only need
        // to set up data.
        space->data = new unsigned char[pLength];
        r_start = space->data;
        if ((m_AccessFlags & AccessMask) != WriteOnly) {
          // Read data from the backend file.
          if (!read(*space)) {
            llvm::report_fatal_error(llvm::Twine("Failed to read data from ") +
                                     m_FilePath.native() +
                                     llvm::Twine(" (") +
                                     sys::fs::detail::strerror(errno) +
                                     llvm::Twine(") at offset ") +
                                     llvm::Twine(pOffset) +
                                     llvm::Twine(" lenght ") +
                                     llvm::Twine(pLength) + llvm::Twine(".\n"));
          }
        }
        break;
      } // case
      default: {
        llvm::report_fatal_error("unhandled space type\n");
      }
    } // switch
  }
  else { // found
    off_t distance = pOffset - space->file_offset;
    r_start = space->data + distance;
  }

  // now, we have a legal space to hold the new MemoryRegion
  return m_RegionFactory.produce(space, r_start, pLength);
}

// release - release a MemoryRegion
void MemoryArea::release(MemoryRegion* pRegion)
{
  if (!isMapped() || !isGood())
    return;

  Space *space = pRegion->parent();
  m_RegionFactory.destruct(pRegion);

  if (0 == space->region_num) {
    write(*space);
    m_SpaceList.remove(*space);
    release(space);
  }
}

void MemoryArea::clean()
{
  m_RegionFactory.clear();

  SpaceList::iterator sIter, sEnd = m_SpaceList.end();
  for (sIter = m_SpaceList.begin(); sIter!=sEnd; ++sIter) {
    write(*sIter);
    release(sIter);
  }
  m_SpaceList.clear();
}

void MemoryArea::sync()
{
  SpaceList::iterator sIter, sEnd = m_SpaceList.end();
  for (sIter = m_SpaceList.begin(); sIter!=sEnd; ++sIter) {
    write(*sIter);
  }
}

MemoryArea::Space* MemoryArea::find(size_t pOffset, size_t pLength)
{
  SpaceList::iterator sIter, sEnd = m_SpaceList.end();
  for (sIter = m_SpaceList.begin(); sIter!=sEnd; ++sIter) {
    if (sIter->file_offset <= pOffset &&
       (pOffset+pLength) <= (sIter->file_offset+sIter->size) ) { // within
      return sIter;
    }
  }
  return NULL;
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
    default:
      break;
  }
}

MemoryArea::Space::Type MemoryArea::policy(off_t pOffset, size_t pLength)
{
  const size_t threshold = (PageSize*3)/4; // 3/4 page size in Linux
  if (pLength < threshold)
    return Space::ALLOCATED_ARRAY;
  else
    return Space::MMAPED;
}

ssize_t MemoryArea::readToBuffer(sys::fs::detail::Address pBuf,
                                 size_t pSize, size_t pOffset) {
  assert(((m_AccessFlags & AccessMask) != WriteOnly) &&
         "Write-only file cannot be read!");

  ssize_t read_bytes = sys::fs::detail::pread(m_FileDescriptor, pBuf,
                                              pSize, pOffset);
  if (static_cast<size_t>(read_bytes) != pSize) {
    // Some error occurred during pread().
    if (read_bytes < 0) {
      m_State |= FailBit;
    }
    else if (static_cast<size_t>(read_bytes) < pSize) {
      m_State |= EOFBit;
      if ((m_AccessFlags & AccessMask) != ReadWrite) {
        // Files which is not read-write are not allowed read beyonds the EOF
        // marker.
        m_State |= BadBit;
      }
    }
    else {
      m_State |= BadBit;
    }
  }
  return read_bytes;
}

bool MemoryArea::read(Space& pSpace) {
  if (!isGood() || !isReadable())
    return false;

  if (pSpace.type == Space::ALLOCATED_ARRAY) {
    readToBuffer(pSpace.data, pSpace.size, pSpace.file_offset);
    return isGood();
  }
  else {
    // Data associated with mmap()'ed space is already at the position the
    // pSpace points to.
    assert((pSpace.type == Space::MMAPED) && "Unknown type of Space!");
    return true;
  }
}


void MemoryArea::write(const Space& pSpace)
{
  if (!isMapped() || !isGood() || !isWritable())
    return;

  switch(pSpace.type) {
    case Space::MMAPED: {
      if(-1 == ::msync(pSpace.data, pSpace.size, MS_SYNC))
        m_State |= FailBit;
      return;
    }
    case Space::ALLOCATED_ARRAY: {
      ssize_t write_bytes = sys::fs::detail::pwrite(m_FileDescriptor,
                                                    pSpace.data,
                                                    pSpace.size,
                                                    pSpace.file_offset);
      if (0 > write_bytes) {
        m_State |= FailBit;
        return;
      }
      if (0 == write_bytes && 0 != pSpace.size)
        m_State |= BadBit;
      if ( pSpace.size > static_cast<size_t>(write_bytes) )
        m_State |= EOFBit;
      return;
    }
    default:
      return;
  }
}

