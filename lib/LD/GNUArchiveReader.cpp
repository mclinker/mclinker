//===- GNUArchiveReader.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLDInputTree.h"
#include "mcld/LD/GNUArchiveReader.h"

#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/system_error.h"

#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace mcld;

typedef size_t sectionSizeTy;
typedef uint32_t elfWord;

/// Archive Header, Magic number, etc..

const int archiveMagicSize = 8;
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

struct GNUArchiveReader::ArchiveMapEntry
{
  off_t fileOffset;
  std::string name;
};


/// Type Conversion

template<class To, class From>
To convertType(const From from)
{
  To to = from;
  assert(static_cast<From>(to) == from);
  return to;
}

template<class From>
inline sectionSizeTy
convertToSectionSizeTy(const From from)
{
  return convertType<sectionSizeTy, From>(from);
}

inline void endian_swap(unsigned int& x)
{
  x = (x>>24) |
      ((x<<8) & 0x00FF0000) |
      ((x>>8) & 0x0000FF00) |
      (x<<24);
}


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
  OwningPtr<MemoryBuffer> mapFile;
  llvm::MemoryBuffer::getFile(pInput.path().c_str(), mapFile);
  const char* pFile = mapFile->getBufferStart();
  
  /// FIXME: compare size_t with int 
  /// check archive format.
  if(mapFile->getBufferSize() <= archiveMagicSize)
    return false;
  bool isThinArchive = memcmp(pFile, thinArchiveMagic, archiveMagicSize) == 0;
  if(!isThinArchive && memcmp(pFile, archiveMagic, archiveMagicSize) != 0)
    return false;
  return true;
}

InputTree *GNUArchiveReader::readArchive(Input &pInput)
{
  OwningPtr<MemoryBuffer> mapFile; 
  mapToMemory(mapFile, pInput.path());
  return setupNewArchive(mapFile, 0);
}

/// Internal used
bool GNUArchiveReader::mapToMemory(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                                  sys::fs::Path archPath)
{ 
  if(llvm::MemoryBuffer::getFile(archPath.c_str(), mapFile)) {
    assert(0=="GNUArchiveReader:can't map a file to MemoryBuffer\n");
    return false;
  }
  return true;
}

InputTree *GNUArchiveReader::setupNewArchive(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                                            size_t off)
{
  std::vector<ArchiveMapEntry> archiveMap;
  std::string archiveMemberName;
  std::string extendedName;
  bool isThinArchive;
  const char *pFile = mapFile->getBufferStart();

  /// FIXME: compare size_t with int 
  /// check archive format.
  if(mapFile->getBufferSize() <= archiveMagicSize)
    return NULL;
  else {
    isThinArchive = memcmp(pFile, thinArchiveMagic, archiveMagicSize) == 0;
    if(!isThinArchive && memcmp(pFile, archiveMagic, archiveMagicSize) != 0) {
      return NULL;
    }
  }

  off += archiveMagicSize ;
  size_t archiveMapSize = parseMemberHeader(mapFile, off, &archiveMemberName, NULL, extendedName);
  /// symbol table header
  if(archiveMemberName.empty()) {
    readArchiveMap(mapFile, archiveMap, off+sizeof(GNUArchiveReader::ArchiveMemberHeader), archiveMapSize);
    off = off + sizeof(GNUArchiveReader::ArchiveMemberHeader) + archiveMapSize;
  }
  else if(true) {
    ///FIXME: check whole-archive from option
  } 
  else {
    assert(0=="fatal error : need symbol table\n");
    return NULL;
  }
  
  if((off&1) != 0)
    ++off; 
  
  size_t extendedSize = parseMemberHeader(mapFile, off, &archiveMemberName, NULL, extendedName);
  /// extended Name table 
  if(archiveMemberName == "/") {
    pFile += off;
    extendedName.assign(pFile,extendedSize);
  }

  /// for each archive Member 
  for(int i=0 ; i<archiveMap.size() ; ++i)
  {
    /// FIXME:
    /// We shall get each member at this archive.
    /// If if a member is the other archive, recursive call setupNewArchive
    /// Finally, construct a corresponding mcld::Input, and insert it into
    /// the original InputTree.
    off_t nestedOff = 0;
    size_t memberOffset = parseMemberHeader(mapFile, archiveMap[i].fileOffset, &archiveMemberName, &nestedOff, extendedName);
    /// normal member ie. relocatable object file.
    /// FIXME: new Input and insert to InputTree
    if(!isThinArchive)
      continue;

    if(nestedOff > 0)
    {
      /// FIXME: recursive call setupNewArchive
      continue;
    }
    ///external member
    ///FIXME: add new Input to InputTree
    ///       But how about the path??

  }
  return NULL; // FIXME: where to return the result??
}


/// Parse the member header and return the size of member
size_t GNUArchiveReader::parseMemberHeader(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                                  off_t off,
                                  std::string *p_Name,
                                  off_t *p_NestedOff,
                                  std::string &p_ExtendedName)
{
  const char *pFile = mapFile->getBufferStart();
  pFile += off;
  const ArchiveMemberHeader *header = reinterpret_cast<const ArchiveMemberHeader *>(pFile);
  
  /// check magic number of member header 
  if(memcmp(header->finalMagic, archiveFinalMagic, sizeof archiveFinalMagic)) {
    assert(0=="archive member header magic number false");
    return 0;
  }

  /// evaluate member size
  std::string sizeString(header->size, sizeof(header->size)+1);
  int memberSize = stringToType<int>(sizeString);
  if(memberSize < 0)
  {
    assert(0=="member Size Error");
    return 0;
  }

  /// switch case for each format.
  /// normal archive member : object file 
  if(header->name[0] == '/')
  {
    const char* nameEnd = strchr(header->name, '/');
    if(nameEnd == NULL || nameEnd - header->name >= static_cast<size_t>(sizeof(header->name)))
    {
      assert(0=="header name format error\n");
      return 0;
    }
    p_Name->assign(header->name, nameEnd - header->name);
    
    if(!p_NestedOff)
      p_NestedOff = 0;
  }
  /// symbol table
  else if(header->name[1] == ' ') {
    if(!p_Name->empty())
      p_Name->clear();
  }
  /// extended name table
  else if(header->name[1] == '/') {
    p_Name->assign(1,'/');
  }
  /// thin archive member
  else {
    std::string nameString(header->name,sizeof(header->name));
    char *end;
    long extendedNameOff = strtol(&nameString[0], &end, 10);
    long nestedOff = 0;  
    if(*end == ':')
      nestedOff = strtol(end+1, &end, 10);

    if(*end != ' ' || 
       extendedNameOff < 0 || 
       static_cast<size_t>(extendedNameOff) >= p_ExtendedName.size()) {
      assert(0=="extended name");
      return 0;
    }

    const char *name = p_ExtendedName.data() + extendedNameOff;
    const char *nameEnd = strchr(name, '\n');
    
    if(nameEnd[-1] != '/' || static_cast<size_t>(nameEnd-name) > p_ExtendedName.size()) {
      assert(0=="p_ExtendedName substring is not end with / \n");
      return 0;
    }

    p_Name->assign(name, nameEnd-name-1);
    if(p_NestedOff)
     *p_NestedOff = nestedOff;
  }
  
  return memberSize;
}

void GNUArchiveReader::readArchiveMap(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile,
                                     std::vector<ArchiveMapEntry> &archiveMap,
                                     off_t start,
                                     size_t size)
{
  const char *startPtr = mapFile->getBufferStart() + start;
  const elfWord *p_Word = reinterpret_cast<const elfWord *>(startPtr);
  unsigned int symbolNum = *p_Word;
  ///FIXME: intel and ARM are littel-endian , Sparc is big-endian
  ///symbolNum read from archive is big-endian 
  ///This is portibility issue.
  endian_swap(symbolNum); 
  ++p_Word;

  const char *p_Name = reinterpret_cast<const char *>(p_Word + symbolNum);
  size_t nameSize = reinterpret_cast<const char *>(startPtr) + size - p_Name;

  archiveMap.resize(symbolNum);
  for(unsigned int i=0 ; i<symbolNum ; ++i)
  {
    /// assign member offset
    unsigned int memberOffset = *p_Word;
    endian_swap(memberOffset);
    archiveMap[i].fileOffset = static_cast<off_t>(memberOffset);
    ++p_Word;
    /// assign member name
    off_t nameEnd = strlen(p_Name) + 1;
    archiveMap[i].name.assign(p_Name, nameEnd);
    p_Name += nameEnd;
  }
}
 
llvm::MemoryBuffer *GNUArchiveReader::getMemberFile(llvm::OwningPtr<llvm::MemoryBuffer> &mapFile, 
                                                   std::vector<ArchiveMapEntry> &archiveMap)
{
  /// FIXME: Do we really need this??
  return NULL;
}
