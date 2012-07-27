//===- GNUArchiveReader.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/InputTree.h>
#include <mcld/LD/GNUArchiveReader.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/ELFObjectReader.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MemoryAreaFactory.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/Path.h>
#include <mcld/ADT/SizeTraits.h>

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Host.h>

#include <cstring>
#include <cstdlib>

using namespace mcld;

GNUArchiveReader::GNUArchiveReader(MCLDInfo& pLDInfo,
                                   MemoryAreaFactory& pMemAreaFactory,
                                   ELFObjectReader& pELFObjectReader)
 : m_LDInfo(pLDInfo),
   m_MemAreaFactory(pMemAreaFactory),
   m_ELFObjectReader(pELFObjectReader)
{
}

GNUArchiveReader::~GNUArchiveReader()
{
}

/// isMyFormat
bool GNUArchiveReader::isMyFormat(Input& pInput) const
{
  assert(pInput.hasMemArea());
  MemoryRegion* region = pInput.memArea()->request(pInput.fileOffset(),
                                                   ARFILE_MAGIC_LEN);
  const char* str = reinterpret_cast<char*>(region->getBuffer());

  bool result = false;
  assert(NULL != str);
  if (isArchive(str) || isThinArchive(str))
    result = true;

  pInput.memArea()->release(region);
  return result;
}

/// isArchive
bool GNUArchiveReader::isArchive(const char* pStr) const
{
  return (0 == memcmp(pStr, ARFILE_MAGIC, ARFILE_MAGIC_LEN));
}

/// isThinArchive
bool GNUArchiveReader::isThinArchive(const char* pStr) const
{
  return (0 == memcmp(pStr, THINAR_MAGIC, ARFILE_MAGIC_LEN));
}

/// isThinArchive
bool GNUArchiveReader::isThinArchive(Input& pInput) const
{
  assert(pInput.hasMemArea());
  MemoryRegion* region = pInput.memArea()->request(pInput.fileOffset(),
                                                   ARFILE_MAGIC_LEN);
  const char* str = reinterpret_cast<char*>(region->getBuffer());

  bool result = false;
  assert(NULL != str);
  if (isThinArchive(str))
    result = true;

  pInput.memArea()->release(region);
  return result;
}

bool GNUArchiveReader::readArchive(Archive& pArchive)
{
  // TODO
  return true;
}

/// readMemberHeader - read the header of a member in a archive file and then
/// return the corresponding archive member (it may be an input object or
/// another archive)
/// @param pArchiveRoot  - the archive root that holds the strtab (extended
///                        name table)
/// @param pArchiveFile  - the archive that contains the needed object
/// @param pFileOffset   - file offset of the member header in the archive
/// @param pNestedOffset - used when we find a nested archive
Input* GNUArchiveReader::readMemberHeader(Archive& pArchiveRoot,
                                          Input& pArchiveFile,
                                          uint32_t pFileOffset,
                                          uint32_t& pNestedOffset)
{
  // TODO
  return NULL;
}

/// readSymbolTable - read the archive symbol map (armap)
bool GNUArchiveReader::readSymbolTable(Archive& pArchive)
{
  // TODO
  return true;
}

/// readStringTable - read the strtab for long file name of the archive
bool GNUArchiveReader::readStringTable(Archive& pArchive)
{
  // TODO
  return true;
}

/// shouldIncludeStatus - given a sym name from armap and check if including
/// the corresponding archive member, and then return the decision
enum Archive::Status
GNUArchiveReader::shouldIncludeSymbol(const llvm::StringRef& pSymName) const
{
  // TODO
  return Archive::Unknown;
}

