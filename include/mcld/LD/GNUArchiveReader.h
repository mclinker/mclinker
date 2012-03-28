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

#include <vector>
#include <string>

namespace llvm
{
class MemoryBuffer;

}

namespace mcld
{
class MemoryArea;
class MCLDInfo;
class Input;
class InputTree;

/** \class GNUArchiveReader
 *  \brief GNUArchiveReader reads GNU archive files.
 */
class GNUArchiveReader : public ArchiveReader
{
private:
  struct SymbolTableEntry;

  enum Constant
  {
    /// The length of the magic strign at the end of an archive member header.
    HeaderFinalMagicSize = 2,
    /// The length of the magic string at the start of an archive.
    ArchiveMagicSize = 8
  };
  /// The magic string at the start of an archive.
  static const char ArchiveMagic[ArchiveMagicSize];
  static const char ThinArchiveMagic[ArchiveMagicSize];
  /// The Magic string expected at the end of an archive member header.
  static const char HeaderFinalMagic[HeaderFinalMagicSize];

public:
  explicit GNUArchiveReader(MCLDInfo &pLDInfo)
  : m_pLDInfo(pLDInfo)
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
  InputTree *setupNewArchive(Input &pInput, size_t off);

  /// read the archive header, and return the member size
  size_t readMemberHeader(MemoryArea &pArea,
                   off_t off,
                   std::string *p_Name,
                   off_t *nestedOff,
                   std::string &p_ExtendedName);

  void readSymbolTable(MemoryArea &pArea,
                      std::vector<SymbolTableEntry> &pSymbolTable,
                      off_t start,
                      size_t size);

private:
  MCLDInfo &m_pLDInfo;
};

} // namespace of mcld

#endif

