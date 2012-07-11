//===- Space.cpp ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/Space.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MsgHandling.h>
#include <cstdlib>
#include <unistd.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// constant data
static const off_t PageSize = getpagesize();

//===----------------------------------------------------------------------===//
// Non-member functions
//
// low address      A page             high address
// |--------------------|------------------|
// ^ page_offset        ^ pFileOffset      ^ page_boundary

// Given a file offset, return the page offset.
// return the first page boundary \b before pFileOffset
inline static off_t page_offset(off_t pFileOffset)
{ return pFileOffset & ~ (PageSize - 1); }

// page_boundary - Given a file size, return the size to read integral pages.
// return the first page boundary \b after pFileOffset
inline static off_t page_boundary(off_t pFileOffset)
{ return (pFileOffset + (PageSize - 1)) & ~ (PageSize - 1); }

inline static Space::Type policy(off_t pOffset, size_t pLength)
{
  const size_t threshold = (PageSize*3)/4; // 3/4 page size in Linux
  if (pLength < threshold)
    return Space::ALLOCATED_ARRAY;
  else
    return Space::MMAPED;
}

//===----------------------------------------------------------------------===//
// Space
Space::Space()
  : m_Data(NULL), m_StartOffset(0), m_Size(0),
    m_RegionCount(0), m_Type(UNALLOCATED) {
}

Space::Space(Space::Type pType, void* pMemBuffer, size_t pSize)
  : m_Data(static_cast<Address>(pMemBuffer)), m_StartOffset(0), m_Size(pSize),
    m_RegionCount(0), m_Type(pType)
{
}

Space::~Space()
{
  // do nothing. m_Data is deleted by @ref releaseSpace
}

Space* Space::createSpace(FileHandle& pHandler,
                          size_t pStart, size_t pSize)
{
  Type type;
  void* memory;
  Space* result = NULL;
  size_t start, size = 0, total_offset;
  switch(type = policy(pStart, pSize)) {
    case ALLOCATED_ARRAY: {
      // adjust total_offset, start and size
      total_offset = pStart + pSize;
      start = pStart;
      if (total_offset > pHandler.size()) {
        if (pHandler.isWritable()) {
          size = pSize;
          pHandler.truncate(total_offset);
        }
        else if (pHandler.size() > start)
          size = pHandler.size() - start;
        else {
          // create a space out of a read-only file.
          fatal(diag::err_cannot_read_small_file) << pHandler.path()
                                                  << pHandler.size()
                                                  << start << size;
        }
      }
      else
        size = pSize;

      // malloc
      memory = (void*)malloc(size);
      if (!pHandler.read(memory, start, size))
        error(diag::err_cannot_read_file) << pHandler.path() << start << size;

      break;
    }
    case MMAPED: {
      // adjust total_offset, start and size
      total_offset = page_boundary(pStart + pSize);
      start = page_offset(pStart);
      if (total_offset > pHandler.size()) {
        if (pHandler.isWritable()) {
          size = page_boundary((pStart - start) + pSize);
          pHandler.truncate(total_offset);
        }
        else if (pHandler.size() > start)
          size = pHandler.size() - start;
        else {
          // create a space out of a read-only file.
          fatal(diag::err_cannot_read_small_file) << pHandler.path()
                                                  << pHandler.size()
                                                  << start << size;
        }
      }
      else
        size = page_boundary((pStart - start) + pSize);

      // mmap
      if (!pHandler.mmap(memory, start, size))
        error(diag::err_cannot_mmap_file) << pHandler.path() << start << size;

      break;
    }
    default:
      break;
  } // end of switch

  result = new Space(type, memory, size);
  result->setStart(start);
  return result;
}

void Space::releaseSpace(Space* pSpace, FileHandle& pHandler)
{
  if (NULL == pSpace)
    return;

  switch(pSpace->type()) {
    case ALLOCATED_ARRAY:
      free(pSpace->memory());
      break;
    case MMAPED:
      if (!pHandler.munmap(pSpace->memory(), pSpace->size()))
        error(diag::err_cannot_munmap_file) << pHandler.path();
      break;
    default: // external and unallocated memory buffers
      break;
  } // end of switch
}

void Space::syncSpace(Space* pSpace, FileHandle& pHandler)
{
  if (NULL == pSpace || !pHandler.isWritable())
    return;

  switch(pSpace->type()) {
    case Space::ALLOCATED_ARRAY: {
      if (!pHandler.write(pSpace->memory(),
                          pSpace->start(),
                          pSpace->size())) {
        error(diag::err_cannot_write_file) << pHandler.path()
                                           << pSpace->start()
                                           << pSpace->size();
      }
      return;
    }
    case Space::MMAPED:
    default: {
      // system will eventually write bakc the memory after
      // calling ::munmap
      return;
    }
  } // end of switch
}

