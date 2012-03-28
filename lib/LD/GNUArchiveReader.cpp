//===- GNUArchiveReader.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCLDInfo.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLDInputTree.h"
#include "mcld/LD/GNUArchiveReader.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/Support/MemoryArea.h"
#include "mcld/Support/MemoryRegion.h"
#include "mcld/Support/MemoryAreaFactory.h"
#include "mcld/ADT/SizeTraits.h"

#include <llvm/Support/system_error.h>
#include <llvm/Support/Host.h>

#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace mcld;

typedef uint32_t elfWord;

/// Archive Header, Magic number, etc..

const unsigned archiveMagicSize = 8;
const char archiveMagic[archiveMagicSize] = { '!', '<', 'a', 'r', 'c', 'h', '>', '\n' };
const char thinArchiveMagic[archiveMagicSize] = { '!', '<', 't', 'h', 'i', 'n', '>', '\n' };
const char archiveFinalMagic[2] = { '`', '\n' };

struct GNUArchiveReader::ArchiveMemberHeader
{
  char name[16];
  char date[12];
  char uid[6];
  char gid[6];
  char mode[8];
  char size[10];
  char finalMagic[2];
};

struct GNUArchiveReader::SymbolTableEntry
{
  off_t fileOffset;
  std::string name;
};



/// convert string to size_t
template<class Type>
Type stringToType(const std::string &str)
{
  Type n;
  std::stringstream ss(str);
  ss >> n;
  return n;
}


/// GNUArchiveReader Operations
/// Public API
bool GNUArchiveReader::isMyFormat(Input &pInput) const
{
  MemoryArea *area = m_pLDInfo.memAreaFactory().produce(pInput.path(), O_RDONLY);
  if (!area)
    llvm::report_fatal_error("can't map file to MemoryArea");

  MemoryRegion *region = area->request(0, archiveMagicSize);
  if (!region)
    llvm::report_fatal_error("can't request MemoryRegion for archive magic");

  const char *p_buffer = reinterpret_cast<char *> (region->getBuffer());

  /// check archive format.
  if (memcmp(p_buffer, archiveMagic, archiveMagicSize) != 0
      && memcmp(p_buffer, thinArchiveMagic, archiveMagicSize) != 0)
    return false;
  return true;
}

LDReader::Endian GNUArchiveReader::endian(Input& pFile) const
{
  return LDReader::BigEndian;
}

InputTree *GNUArchiveReader::readArchive(Input &pInput)
{
  return setupNewArchive(pInput, 0);
}

/// Read Input as archive. First create a null InputTree.
/// Then Construct Input object for corresponding member of this archive
/// and insert the Input object into the InputTree.
/// Finally, return the InputTree.
InputTree *GNUArchiveReader::setupNewArchive(Input &pInput,
                                            size_t off)
{
  MemoryArea *area = m_pLDInfo.memAreaFactory().produce(pInput.path(), O_RDONLY);
  if (!area)
    llvm::report_fatal_error("can't map file to MemoryArea");

  MemoryRegion *region = area->request(off, archiveMagicSize);
  if (!region)
    llvm::report_fatal_error("can't request MemoryRegion for archive magic");

  const char *pFile = reinterpret_cast<char *> (region->getBuffer());

  /// check archive format.
  bool isThinArchive;
  isThinArchive = memcmp(pFile, thinArchiveMagic, archiveMagicSize) == 0;
  if(!isThinArchive && memcmp(pFile, archiveMagic, archiveMagicSize) != 0)
    return NULL;

  InputTree *resultTree = new InputTree(m_pLDInfo.inputFactory());
  std::vector<SymbolTableEntry> symbolTable;
  std::string archiveMemberName;
  std::string extendedName;

  off += archiveMagicSize ;
  size_t symbolTableSize = readMemberHeader(*area, off, &archiveMemberName,
                                            NULL, extendedName);
  /// read archive symbol table
  if(archiveMemberName.empty())
  {
    readSymbolTable(*area, symbolTable,
                    off+sizeof(GNUArchiveReader::ArchiveMemberHeader), symbolTableSize);
    off = off + sizeof(GNUArchiveReader::ArchiveMemberHeader) + symbolTableSize;
  }
  else
  {
    llvm::report_fatal_error("fatal error : need symbol table\n");
    return NULL;
  }

  if((off&1) != 0)
    ++off;

  size_t extendedSize = readMemberHeader(*area, off, &archiveMemberName,
                                          NULL, extendedName);
  /// read long Name table if exist
  if(archiveMemberName == "/")
  {
    off += sizeof(GNUArchiveReader::ArchiveMemberHeader);
    MemoryRegion *extended_name_region = area->request(off, extendedSize);
    pFile = reinterpret_cast<char *>(extended_name_region->getBuffer());
    extendedName.assign(pFile, extendedSize);

  }

  /// traverse all the archive members
  InputTree::iterator node = resultTree->root();
  std::set<std::string> haveSeen;
  for(unsigned i=0 ; i<symbolTable.size() ; ++i)
  {
    /// We shall get each member at this archive.
    /// Construct a corresponding mcld::Input, and insert it into
    /// the original InputTree, resultTree.
    off_t nestedOff = 0;

    readMemberHeader(*area, symbolTable[i].fileOffset, &archiveMemberName,
                      &nestedOff, extendedName);

    if(haveSeen.find(archiveMemberName)==haveSeen.end())
      haveSeen.insert(archiveMemberName);
    else
      continue;

    if(!isThinArchive)
    {
      /// New a Input object and assign fileOffset in MCLDFile.
      /// Insert the object to resultTree and move ahead.
      off_t fileOffset = symbolTable[i].fileOffset + sizeof(ArchiveMemberHeader);
      Input *insertObjectFile = m_pLDInfo.inputFactory().produce(archiveMemberName,
                                                                 pInput.path(),
                                                                 MCLDFile::Object,
                                                                 fileOffset);
      resultTree->insert<InputTree::Positional>(node, *insertObjectFile);
      if(i==0)
        node.move<InputTree::Inclusive>();
      else
        node.move<InputTree::Positional>();

      continue;
    }

    /// create the real path
    sys::fs::RealPath realPath(archiveMemberName);
    if(nestedOff > 0)
    {
      /// This is a member of a nested archive.
      /// Create an Input for this archive ,and recursive call setupNewArchive
      /// Finally, merge the new InputTree with the old one
      Input *newArchive = m_pLDInfo.inputFactory().produce(archiveMemberName,
                                                           realPath,
                                                           MCLDFile::Archive,
                                                           0);

      resultTree->insert<InputTree::Positional>(node, *newArchive);
      if(i==0)
        node.move<InputTree::Inclusive>();
      else
        node.move<InputTree::Positional>();
      InputTree *newArchiveTree = setupNewArchive(*newArchive, 0);
      resultTree->merge<InputTree::Inclusive>(node, *newArchiveTree);
      continue;
    }
    /// External member , open it as normal object file
    /// add new Input to InputTree
    Input *insertObjectFile = m_pLDInfo.inputFactory().produce(archiveMemberName,
                                                               realPath,
                                                               MCLDFile::Object,
                                                               0);
    resultTree->insert<InputTree::Positional>(node, *insertObjectFile);
    if(i==0)
      node.move<InputTree::Inclusive>();
    else
      node.move<InputTree::Positional>();
  }
  return resultTree;
}


/// Parse the member header and return the size of member
/// Archive member names in System 5 style :
///
/// "/                  " - symbol table, must be the first member
/// "//                 " - long name table
/// "filename.o/        " - regular file with short name
/// "/5566              " - name at offset 5566 at long name table

size_t GNUArchiveReader::readMemberHeader(MemoryArea &pArea,
                                           off_t off,
                                           std::string *p_Name,
                                           off_t *p_NestedOff,
                                           std::string &p_ExtendedName)
{
  MemoryRegion *region = pArea.request(off, sizeof(ArchiveMemberHeader));
  const char *pFile = reinterpret_cast<char *>(region->getBuffer());
  const ArchiveMemberHeader *header = reinterpret_cast<const ArchiveMemberHeader *>(pFile);

  /// check magic number of member header
  if(memcmp(header->finalMagic, archiveFinalMagic, sizeof archiveFinalMagic))
  {
    llvm::report_fatal_error("archive member header magic number false");
    return 0;
  }

  /// evaluate member size
  std::string sizeString(header->size, sizeof(header->size)+1);
  size_t memberSize = stringToType<size_t>(sizeString);
  if(memberSize == 0)
  {
    llvm::report_fatal_error("member Size Error");
    return 0;
  }

  if(header->name[0] != '/')
  {
    /// This is a regular file with short name
    const char* nameEnd = strchr(header->name, '/');
    size_t nameLen = ((nameEnd == NULL) ? 0 : (nameEnd - header->name));
    if((nameLen <= 0) || (nameLen >= sizeof(header->name)))
    {
      llvm::report_fatal_error("header name format error\n");
      return 0;
    }
    p_Name->assign(header->name, nameLen);

    if(!p_NestedOff)
      p_NestedOff = 0;
  }
  else if(header->name[1] == ' ')
  {
    /// This is symbol table
    if(!p_Name->empty())
      p_Name->clear();
  }
  else if(header->name[1] == '/')
  {
    /// This is long name table
    p_Name->assign(1,'/');
  }
  else
  {
    /// This is regular file with long name
    char *end;
    long extendedNameOff = strtol(header->name+1, &end, 10);
    long nestedOff = 0;
    if(*end == ':')
      nestedOff = strtol(end+1, &end, 10);

    if(*end != ' '
       || extendedNameOff < 0
       || static_cast<size_t>(extendedNameOff) >= p_ExtendedName.size())
    {
      llvm::report_fatal_error("extended name");
      return 0;
    }

    const char *name = p_ExtendedName.data() + extendedNameOff;
    const char *nameEnd = strchr(name, '\n');
    if(nameEnd[-1] != '/'
       || static_cast<size_t>(nameEnd-name) > p_ExtendedName.size())
    {
      llvm::report_fatal_error("p_ExtendedName substring is not end with / \n");
      return 0;
    }
    p_Name->assign(name, nameEnd-name-1);
    if(p_NestedOff)
     *p_NestedOff = nestedOff;
  }

  return memberSize;
}

void GNUArchiveReader::readSymbolTable(MemoryArea &pArea,
                                       std::vector<SymbolTableEntry> &pSymbolTable,
                                       off_t start,
                                       size_t size)
{
  MemoryRegion *region = pArea.request(start, size);
  const char *pFile = reinterpret_cast<char *>(region->getBuffer());
  const elfWord *p_Word = reinterpret_cast<const elfWord *>(pFile);
  unsigned int symbolNum = *p_Word;

  /// Portable Issue on Sparc platform
  /// Intel, ARM and Mips are littel-endian , Sparc is little-endian after verion 9
  /// symbolNum in symbol table is always big-endian
  if(llvm::sys::isLittleEndianHost())
    symbolNum = bswap32(symbolNum);
  ++p_Word;

  const char *p_Name = reinterpret_cast<const char *>(p_Word + symbolNum);

  pSymbolTable.resize(symbolNum);
  for(unsigned int i=0 ; i<symbolNum ; ++i)
  {
    /// assign member offset
    unsigned int memberOffset = *p_Word;
    if(llvm::sys::isLittleEndianHost())
      memberOffset = bswap32(memberOffset);
    pSymbolTable[i].fileOffset = static_cast<off_t>(memberOffset);
    ++p_Word;
    /// assign member name
    off_t nameEnd = strlen(p_Name) + 1;
    pSymbolTable[i].name.assign(p_Name, nameEnd);
    p_Name += nameEnd;
  }
}
