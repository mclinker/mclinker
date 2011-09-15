/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_MEMORY_AREA_H
#define MCLD_MEMORY_AREA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/Uncopyable.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/Path.h>
#include <llvm/ADT/ilist.h>
#include <llvm/ADT/ilist_node.h>
#include <string>

namespace mcld
{

class MemoryRegion;

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
friend class MemoryAreaTest;
#endif

private:
  typedef sys::fs::detail::Address Address;

  friend class MemoryRegion;
  struct Space : public llvm::ilist_node<Space>
  {
  public:
    enum Type
    {
      ALLOCATED_ARRAY,
      MMAPED,
    };

  public:  
    Type type;
    sys::fs::detail::Address data;
    size_t size;
    off_t file_offset;
    unsigned int region_counter;
  };

  typedef llvm::iplist<Space> SpaceList;

public:
  MemoryArea(const sys::fs::Path& pPath);
  ~MemoryArea();

  // request - create a MemoryRegion within a sufficient space
  // find an existing space to hold the MemoryRegion.
  // if MemoryArea does not find such space, then it creates a new space and
  // assign a MemoryRegion into the space.
  MemoryRegion* request(off_t pOffset, size_t pLength);

  // release - release a MemoryRegion.
  // if the space has no MemoryRegion, then release the space too.
  void release(MemoryRegion* pRegion);

  // clean - release all MemoryRegion and unmap all spaces.
  void clean();

  void open(const sys::fs::Path& pPath, int flags);
  void open(const sys::fs::Path& pPath, int flags, int mode);
  void close();
  bool isGood() const;

private:
  // find - first fit search
  Space* find(off_t pOffset, size_t pLength);

  void release(Space* pSpace);

  // policy - decide whehter to use dynamic memory or memory mapped I/O
  Space::Type policy(off_t pOffset, size_t pLength);

private:
  int m_FileDescriptor;
  SpaceList m_SpaceList;
  const sys::fs::Path& m_FilePath;

};

} // namespace of mcld

#endif

