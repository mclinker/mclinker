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

#include "mcld/ADT/Uncopyable.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/Support/Path.h"
#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
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
#if defined(ENABLE_UNITTEST)
friend class mcldtest::MemoryAreaTest;
#endif
public:
  enum IOState
  {
    GoodBit    = 0,
    BadBit     = 1L << 0,
    EOFBit     = 1L << 1,
    FailBit    = 1L << 2,
    IOStateEnd = 1L << 16
  };

  enum AccessMode
  {
    ReadOnly = O_RDONLY,
    WriteOnly = O_WRONLY,
    ReadWrite = O_RDWR,
    AccessMask = O_ACCMODE
  };

private:
  typedef sys::fs::detail::Address Address;

  friend class MemoryRegion;
  friend class RegionFactory;
  struct Space : public llvm::ilist_node<Space>
  {
  public:
    enum Type
    {
      ALLOCATED_ARRAY,
      MMAPED,
      UNALLOCATED
    };

  public:
    Space()
    : m_pParent(NULL),
      type(UNALLOCATED),
      file_offset(0),
      size(0),
      data(0),
      region_num(0)
    { }

    Space(MemoryArea* pParent, size_t pOffset, size_t pLength)
    : m_pParent(pParent),
      type(UNALLOCATED),
      file_offset(pOffset),
      size(pLength),
      data(0),
      region_num(0)
    { }

    ~Space()
    { }

    void sync()
    { m_pParent->write(*this); }

  private:
    MemoryArea* m_pParent;

  public:
    Type type;
    size_t file_offset;
    size_t size;
    sys::fs::detail::Address data;
    size_t region_num;
  };

  friend class Space;
  typedef llvm::iplist<Space> SpaceList;

public:
  // constructor
  // @param pRegionFactory the factory to manage MemoryRegions
  MemoryArea(RegionFactory& pRegionFactory);

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

  // clean - release all MemoryRegion and unmap all spaces.
  void clean();

  // sync - sync all MemoryRegion
  void sync();

  // map - open the file pPath and mapped it onto MemoryArea
  // @param flags see man 2 open
  void map(const sys::fs::Path& pPath, int flags);

  // map - open the file pPath and mapped it onto MemoryArea
  // @param flags see man 2 open
  // @param mode see man 2 open
  void map(const sys::fs::Path& pPath, int flags, int mode);

  // unmap - close the opened file and unmap the MemoryArea
  void unmap();

  // path - the path of the mapped file.
  const sys::fs::Path& path() const
  { return m_FilePath; }

  // size - the real size of the mapped file.
  size_t size() const
  { return m_FileSize; }

  // isMapped - check if MemoryArea is mapped to a file
  bool isMapped() const;

  // isGood - check if the state of the opened area is good for read/write
  // operations
  bool isGood() const;

  // isBad - check if an error causes the loss of integrity of the memory space
  bool isBad() const;

  // isFailed - check if an error related to the internal logic of the operation
  // itself occurs
  bool isFailed() const;

  // isEOF - check if we reach the end of the file
  bool isEOF() const;

  // isReadable - check if the memory area is readable
  bool isReadable() const;

  // isWriteable - check if the memory area is writable
  bool isWritable() const;

  // rdstate - get error state flags
  // Returns the current internal error state flags of the stream
  int rdstate() const;

  // setState - set error state flag
  void setState(IOState pState);

  // clear - set error state flag
  void clear(IOState pState = GoodBit);

private:
  // readToBuffer - read data from the file behind this MemorySpace and store
  // those bytes in pBuf. Return the number of byte read or -1 on error.
  ssize_t readToBuffer(sys::fs::detail::Address pBuf,
                       size_t pSize, size_t pOffset);

private:
  // find - first fit search
  Space* find(size_t pOffset, size_t pLength);

  // release a Space, but does not remove it from space list
  void release(Space* pSpace);

  // read - read data from mapped file into virtual memroy of pSpace. Return
  // false on error.
  bool read(Space& pSpace);

  // write - write back the virtual memory of pSpace into mapped file.
  void write(const Space& pSpace);

  // truncate - truncate the file size to length.
  void truncate(size_t pLength);

  // policy - decide whehter to use dynamic memory or memory mapped I/O
  Space::Type policy(off_t pOffset, size_t pLength);

  // the size of one page
  static const off_t PageSize = 4096;

  // page_boundary - Given a file size, return the size to read integral pages.
  // return the first page boundary after pFileOffset
  static off_t page_boundary(off_t pFileOffset)
  { return (pFileOffset + (PageSize - 1)) & ~ (PageSize - 1); }

  // Given a file offset, return the page offset.
  // return the first page boundary before pFileOffset
  static off_t page_offset(off_t pFileOffset)
  { return pFileOffset & ~ (PageSize - 1); }

private:
  RegionFactory& m_RegionFactory;
  sys::fs::Path m_FilePath;
  int m_FileDescriptor;
  size_t m_FileSize;
  int m_AccessFlags;
  int m_State;

  SpaceList m_SpaceList;
};

} // namespace of mcld

#endif

