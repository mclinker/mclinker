//===- LinkerScript.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LINKER_SCRIPT_H
#define MCLD_LINKER_SCRIPT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <vector>
#include <llvm/ADT/StringRef.h>
#include <mcld/ADT/StringEntry.h>
#include <mcld/ADT/StringHash.h>
#include <mcld/ADT/HashTable.h>
#include <mcld/Object/SectionMap.h>
#include <mcld/MC/SearchDirs.h>
#include <mcld/Script/Assignment.h>

namespace mcld {

class LDSymbol;

/** \class LinkerScript
 *
 */
class LinkerScript
{
public:
  typedef HashTable<StringEntry<llvm::StringRef>,
                    hash::StringHash<hash::ELF>,
                    StringEntryFactory<llvm::StringRef> > SymbolRenameMap;

  typedef HashTable<StringEntry<uint64_t>,
                    hash::StringHash<hash::ELF>,
                    StringEntryFactory<uint64_t> > AddressMap;

  typedef std::vector<std::pair<LDSymbol*, Assignment> > Assignments;

  class DefSyms {
  public:
    void append(const std::string& pDefSym)
    {
      m_DefSymList.append(pDefSym).append(";");
    }
    const std::string& data() const { return m_DefSymList; }
    std::string&       data()       { return m_DefSymList; }

  private:
    std::string m_DefSymList;
  };

public:
  LinkerScript();

  ~LinkerScript();

  const SymbolRenameMap& renameMap() const { return m_SymbolRenames; }
  SymbolRenameMap&       renameMap()       { return m_SymbolRenames; }

  const AddressMap& addressMap() const { return m_AddressMap; }
  AddressMap&       addressMap()       { return m_AddressMap; }

  const SectionMap& sectionMap() const { return m_SectionMap; }
  SectionMap&       sectionMap()       { return m_SectionMap; }

  const Assignments& assignments() const { return m_Assignments; }
  Assignments&       assignments()       { return m_Assignments; }

  // --defsym
  const DefSyms& defSyms() const { return m_DefSyms; }
  DefSyms&       defSyms()       { return m_DefSyms; }

  /// search directory
  const SearchDirs& directories() const { return m_SearchDirs; }
  SearchDirs&       directories()       { return m_SearchDirs; }

  /// sysroot
  const sys::fs::Path& sysroot() const;

  void setSysroot(const sys::fs::Path &pPath);

  bool hasSysroot() const;

  /// entry point
  const std::string& entry() const;

  void setEntry(const std::string& pEntry);

  bool hasEntry() const;

private:
  SymbolRenameMap m_SymbolRenames;
  AddressMap m_AddressMap;
  SectionMap m_SectionMap;
  Assignments m_Assignments;
  DefSyms m_DefSyms;
  SearchDirs m_SearchDirs;
  std::string m_Entry;
};

} // namespace of mcld

#endif

