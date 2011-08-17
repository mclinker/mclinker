/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCARCHIVEREADER_H
#define MCARCHIVEREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Support/Path.h>

#include "llvm/ADT/OwningPtr.h"

#include <vector>
#include <string>

namespace llvm
{
class MemoryBuffer;
}

namespace mcld
{
class InputTree;
class Input;

/** \class MCArchiveReader
 *  \brief MCArchiveReader provides the interface of reading an archive.
 *
 *  \see TargetArchiveReader
 *  \author Duo <pinronglu@gmail.com>
 */
class MCArchiveReader
{
private:
  struct ArchiveMemberHeader;
  struct ArchiveMapEntry;

public:
  virtual ~MCArchiveReader()
  { }
  /// Read an archive and extract each member in.
  /// Construct the coresponding Input for each member.
  InputTree *readArchive(Input &input);

  bool isMyFormat(Input &input);

private:
  /// Map file to MemoryBuffer
  bool mapToMemory(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                   sys::fs::Path archPath);
 
  /// set up the archive, including 
  /// first, read symbol table 
  /// second, read extended file name which is used in thin archive
  InputTree *setupNewArchive(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                             size_t off);

  /// parse the archive header, and return the member size
  size_t parseMemberHeader(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                   off_t off,
                   std::string *p_Name,
                   off_t *nestedOff,
                   std::string &p_ExtendedName);

  void readArchiveMap(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                      std::vector<ArchiveMapEntry> &archiveMap,
                      off_t start,
                      size_t size); 

  llvm::MemoryBuffer *getMemberFile(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                                    std::vector<ArchiveMapEntry> &archiveMap); 

};

} // namespace of mcld

#endif

