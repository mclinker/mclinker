/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLDFILE_H
#define MCLDFILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include <string>
#include <list>
#include <memory>

#include "mcld/MC/MCLDContext.h"

namespace mcld
{
class MCContext;
class MCLDContext;

/** \class MCLDFile
 *  \brief MCLDFile represents the file being linked or produced.
 *
 *  \see llvm::sys::Path
 */
class MCLDFile : public llvm::sys::Path
{
public:
  bool isRecognized() const;
  const MCSectionELF *getELFSection(StringRef Section, unsigned Type,
                                    unsigned Flags, SectionKind Kind,
                                    unsigned EntrySize, StringRef Group);

  MCLDContext *m_pContext;

private:
  std::string m_AbsFilePath;
  std::string m_ArgName;
};

} // namespace of mcld

namespace std
{

/** \class StackAllocator
 *  \brief a stack allocator which create a new chunk while it has not enough
 *   memory.
 *
 *  This allocator is designed for a sequence of allocations and 
 *  deallocations. It also releases all memory before destruction.
 */
class StackAllocator
{
protected:
  enum { MaxBytes = 128 };
  enum { Align = sizeof(pointer) };
  enum { ChunkSize = (size_t)MaxBytes/(size_t)Align };

protected:
  struct Chunk {
    char* volatile m_FreeList
  };
  static char* volatile m_FreeList[ChunkSize];
};

/** \class MCLDFileFactory
 *  \brief The allocator of MCLDFile
 *
 *  \see llvm::sys::Path
 */
template<>
class allocator<mcld::MCLDFile> : private ChunkAllocator
{
public:
  typedef mcld::MCLDFile        value_type;
  typedef mcld::MCLDFile*       pointer;
  typedef mcld::MCLDFile&       reference;
  typedef const mcld::MCLDFile* const_pointer;
  typedef const mcld::MCLDFile& const_reference;
  typedef size_t                size_type;
  typedef ptrdiff_t             difference_type;

public:
  allocator() {
  }

  allocator(const allocator& pCopy)
    : m_FileList() {
  }

  ~allocator() {
    LDFileList::iterator file, fEnd = m_FileList.end();
    for (file = m_FileList.begin(); file!=fEnd; ++file) {
      delete (*file);
    }
  }

  pointer address(reference X) const
  { return &X; }

  const_pointer address(const_reference X) const
  { return &X;  }

  pointer allocate(size_type N, const void* = 0) {
  }

  void deallocate(pointer pPtr, size_type N) {
  }

  size_type max_size() const {
    return m_FileList.size();
  }

  void construct(pointer pPtr, const_reference pVal) {
  }

  void destroy(pointer pPtr) {
  }

  /// delegate - move our own data to the master
  void delegate(allocator& pMaster) {
  }

private:
  typedef std::list<mcld::MCLDFile*> LDFileList;

private:
  LDFileList m_FileList;
};


} // namespace of std
#endif

