//===- GNUArchiveReader.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_GNU_ARCHIVE_READER_H
#define MCLD_GNU_ARCHIVE_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/LD/ArchiveReader.h"
#include "mcld/Support/Path.h"
#include <llvm/ADT/OwningPtr.h>

#include <vector>
#include <string>

namespace llvm
{
class MemoryBuffer;

}

namespace mcld
{
class MCLDInfo;
class Input;
class InputTree;

/** \class GNUArchiveReader
 *  \brief GNUArchiveReader reads GNU archive files.
 */
class GNUArchiveReader : public ArchiveReader
{
private:
  struct ArchiveMemberHeader;
  struct ArchiveMapEntry;

public:
  //FIXME:temp use ,still don't know how to get MCLDInfo from GNULDBackend.
  GNUArchiveReader()
  : m_pInfo(0),
    m_endian(LDReader::LittleEndian)
  {
  }

  GNUArchiveReader(LDReader::Endian endian, MCLDInfo *info)
  : m_pInfo(info),
    m_endian(endian)
  { }

  ~GNUArchiveReader()
  { }

  /// Read an archive and extract each member in.
  /// Construct the coresponding Input for each member.
  InputTree *readArchive(Input &input);

  bool isMyFormat(Input &input) const;

  LDReader::Endian endian(Input& pFile) const;

private:
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

private:
  MCLDInfo *m_pInfo;
  LDReader::Endian m_endian;  
};

} // namespace of mcld

#endif

