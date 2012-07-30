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

#include <mcld/LD/ArchiveReader.h>
#include <mcld/LD/Archive.h>

namespace mcld
{
class MemoryAreaFactory;
class MCLDInfo;
class Input;
class ELFObjectReader;
class Archive;

/** \class GNUArchiveReader
 *  \brief GNUArchiveReader reads GNU archive files.
 */
class GNUArchiveReader : public ArchiveReader
{
public:
  explicit GNUArchiveReader(MCLDInfo& pLDInfo,
                            MemoryAreaFactory& pMemAreaFactory,
                            ELFObjectReader& pELFObjectReader);

  ~GNUArchiveReader();

  /// readArchive - read an archive, include the needed members, and build up
  /// the subtree
  bool readArchive(Archive& pArchive);

  /// isMyFormat
  bool isMyFormat(Input& input) const;

private:
  /// isArchive
  bool isArchive(const char* pStr) const;

  /// isThinArchive
  bool isThinArchive(const char* pStr) const;

  /// isThinArchive
  bool isThinArchive(Input& input) const;

  /// readMemberHeader - read the header of a member in a archive file and then
  /// return the corresponding archive member (it may be an input object or
  /// another archive)
  /// @param pArchiveRoot  - the archive root that holds the strtab (extended
  ///                        name table)
  /// @param pArchiveFile  - the archive that contains the needed object
  /// @param pFileOffset   - file offset of the member header in the archive
  /// @param pNestedOffset - used when we find a nested archive
  Input* readMemberHeader(Archive& pArchiveRoot,
                          Input& pArchiveFile,
                          uint32_t pFileOffset,
                          uint32_t& pNestedOffset);

  /// readSymbolTable - read the archive symbol map (armap)
  bool readSymbolTable(Archive& pArchive);

  /// readStringTable - read the strtab for long file name of the archive
  bool readStringTable(Archive& pArchive);

  /// shouldIncludeSymbol - given a sym name from armap and check if we should
  /// include the corresponding archive member, and then return the decision
  enum Archive::Symbol::Status
  shouldIncludeSymbol(const llvm::StringRef& pSymName) const;

private:
  MCLDInfo& m_LDInfo;
  MemoryAreaFactory& m_MemAreaFactory;
  ELFObjectReader& m_ELFObjectReader;
};

} // namespace of mcld

#endif

