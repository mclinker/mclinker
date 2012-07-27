//===- Archive.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARCHIVE_H
#define MCLD_ARCHIVE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/HashEntry.h>
#include <mcld/ADT/HashTable.h>
#include <mcld/ADT/StringHash.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/MC/InputTree.h>
#include <llvm/ADT/StringRef.h>

#include <vector>
#include <string>

#define ARFILE_MAGIC "!<arch>\n"                   ///< magic string
#define THINAR_MAGIC "!<thin>\n"                   ///< magic of thin archive
#define ARFILE_MAGIC_LEN (sizeof(ARFILE_MAGIC)-1)  ///< length of magic string
#define ARFILE_SVR4_SYMTAB_NAME "/               " ///< SVR4 symtab entry name
#define ARFILE_STRTAB_NAME      "//              " ///< Name of string table
#define ARFILE_PAD "\n"                            ///< inter-file align padding
#define ARFILE_MEMBER_MAGIC "`\n"                  ///< fmag field magic #

namespace mcld
{
class InputTree;
class Input;

/** \class Archive
 *  \brief This class define the interfacee to Archive files
 */
class Archive
{
public:
  struct MemberHeader
  {
    char name[16];  ///< Name of the file member.
    char date[12];  ///< File date, decimal seconds since Epoch
    char uid[6];    ///< user id in ASCII decimal
    char gid[6];    ///< group id in ASCII decimal
    char mode[8];   ///< file mode in ASCII octal
    char size[10];  ///< file size in ASCII decimal
    char fmag[2];   ///< Always contains ARFILE_MAGIC_TERMINATOR
  };

public:
  Archive(Input& pInputFile, InputFactory& pInputFactory);

  ~Archive();

  /// getARFile - get the Input& of the archive file
  Input& getARFile();

  /// getARFile - get the Input& of the archive file
  const Input& getARFile() const;

  /// inputs - get the input tree built from this archive 
  InputTree& inputs();

  /// inputs - get the input tree built from this archive 
  const InputTree& inputs() const;

private:
  template<typename OFFSET_TYPE>
  struct OffsetCompare
  {
    bool operator()(OFFSET_TYPE X, OFFSET_TYPE Y) const
    { return (X == Y); }
  };

  struct MurmurHash3
  {
    size_t operator()(uint32_t pKey) const
    {
      size_t h;
      h ^= h >> 16;
      h *= 0x85ebca6b;
      h ^= h >> 13;
      h *= 0xc2b2ae35;
      h ^= h >> 16;
      return h;
    }
  };

  typedef HashEntry<uint32_t,
                    InputTree::iterator,
                    OffsetCompare<uint32_t> > ObjectMemberEntryType;
public:
  typedef HashTable<ObjectMemberEntryType,
                    MurmurHash3,
                    EntryFactory<ObjectMemberEntryType> > ObjectMemberMapType;

public:
  /// getObjectMemberMap - get the map that contains the included object files
  ObjectMemberMapType& getObjectMemberMap();

  /// getObjectMemberMap - get the map that contains the included object files
  const ObjectMemberMapType& getObjectMemberMap() const;

  /// numOfObjectMember - return the number of included object files
  size_t numOfObjectMember() const;

  /// addObjectMember - add a object in the object member map
  /// @param pFileOffset - file offset in symtab represents a object file
  /// @param pIter - the iterator in the input tree built from this archive
  bool addObjectMember(uint32_t pFileOffset, InputTree::iterator pIter);

  /// hasObjectMember - check if a object file is included or not
  /// @param pFileOffset - file offset in symtab represents a object file
  bool hasObjectMember(uint32_t pFileOffset) const;

public:
  struct ArchiveMember
  {
    Input* file;
    InputTree::iterator lastPos;
    InputTree::Mover* move;
  };
  typedef struct ArchiveMember ArchiveMemberType;

private:
  typedef HashEntry<const llvm::StringRef,
                    ArchiveMember,
                    StringCompare<llvm::StringRef> > ArchiveMemberEntryType;

public:
  typedef HashTable<ArchiveMemberEntryType,
                    StringHash<ELF>,
                    EntryFactory<ArchiveMemberEntryType> > ArchiveMemberMapType;

  /// getArchiveMemberMap - get the map that contains the included archive files
  ArchiveMemberMapType& getArchiveMemberMap();

  /// getArchiveMemberMap - get the map that contains the included archive files
  const ArchiveMemberMapType& getArchiveMemberMap() const;

  /// addArchiveMember - add an archive in the archive member map
  /// @param pName    - the name of the new archive member
  /// @param pLastPos - this records the point to insert the next node in the
  ///                   subtree of this archive member
  /// @param pMove    - this records the direction to insert the next node in
  ///                   the subtree of this archive member
  bool addArchiveMember(const llvm::StringRef& pName,
                        InputTree::iterator pLastPos,
                        InputTree::Mover* pMove);

  /// hasArchiveMember - check if an archive file is included or not
  bool hasArchiveMember(const llvm::StringRef& pName) const;

  /// getArchiveMember - get a archive member
  ArchiveMemberType* getArchiveMember(const llvm::StringRef& pName);

public:
  enum Status 
  {
    Include,
    Exclude,
    Unknown
  };

  struct SymTabEntry
  {
  public:
    SymTabEntry(const char* pName,
                uint32_t pOffset,
                enum Status pStatus)
     : name(pName), fileOffset(pOffset), status(pStatus)
    {}

    ~SymTabEntry()
    {}

  public:
    const std::string name;
    uint32_t fileOffset;
    enum Status status;
  };
  typedef struct SymTabEntry SymTabEntry;

  typedef std::vector<SymTabEntry*> SymTabType;

public:
  /// getSymbolTable - get the symtab
  SymTabType& getSymbolTable();

  /// getSymbolTable - get the symtab
  const SymTabType& getSymbolTable() const;

  /// setSymTabSize - set the memory size of symtab
  void setSymTabSize(size_t pSize);

  /// getSymTabSize - get the memory size of symtab
  size_t getSymTabSize() const;

  /// numOfSymbols - return the number of symbols in symtab
  size_t numOfSymbols() const;

  /// addSymbol - add a symtab entry to symtab
  /// @param pName - symbol name
  /// @param pFileOffset - file offset in symtab represents a object file
  void addSymbol(const char* pName,
                 uint32_t pFileOffset,
                 enum Status pStatus = Archive::Unknown);

  /// getSymbolName - get the symbol name with the given index
  const std::string& getSymbolName(size_t pSymIdx) const;

  /// getObjFileOffset - get the file offset that represent a object file
  uint32_t getObjFileOffset(size_t pSymIdx) const;

  /// getSymbolStatus - get the status of a symbol
  enum Status getSymbolStatus(size_t pSymIdx) const;

  /// setSymbolStatus - set the status of a symbol
  void setSymbolStatus(size_t pSymIdx, enum Status pStatus);

  /// getStrTable - get the extended name table
  std::string& getStrTable();

  /// getStrTable - get the extended name table
  const std::string& getStrTable() const;

private:
  typedef GCFactory<SymTabEntry, 0> SymTabEntryFactory;

private:
  Input& m_ArchiveFile;
  InputTree *m_pInputTree;
  ObjectMemberMapType m_ObjectMemberMap;
  ArchiveMemberMapType m_ArchiveMemberMap;
  SymTabEntryFactory m_SymTabEntryFactory;
  SymTabType m_SymTab;
  size_t m_SymTabSize;
  std::string m_StrTab;
};

} // namespace of mcld

#endif

