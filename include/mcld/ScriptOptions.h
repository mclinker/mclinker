//===- ScriptOptions.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_OPTIONS_H
#define MCLD_SCRIPT_OPTIONS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <llvm/ADT/StringRef.h>
#include <mcld/ADT/StringEntry.h>
#include <mcld/ADT/StringHash.h>
#include <mcld/ADT/HashTable.h>

namespace mcld {

/** \class ScriptOptions
 *
 */
class ScriptOptions
{
public:
  typedef HashTable<StringEntry<llvm::StringRef>,
                    StringHash<ELF>,
                    StringEntryFactory<llvm::StringRef> > SymbolRenameMap;

  typedef HashTable<StringEntry<uint64_t>,
                    StringHash<ELF>,
                    StringEntryFactory<uint64_t> > AddressMap;

public:
  ScriptOptions();

  ~ScriptOptions();

  const SymbolRenameMap& renameMap() const { return m_SymbolRenames; }
  SymbolRenameMap&       renameMap()       { return m_SymbolRenames; }

  const AddressMap& addressMap() const { return m_AddressMap; }
  AddressMap&       addressMap()       { return m_AddressMap; }

private:
  SymbolRenameMap m_SymbolRenames;
  AddressMap m_AddressMap;
};

} // namespace of mcld

#endif

