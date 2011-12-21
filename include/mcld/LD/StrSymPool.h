//===- StrSymPool.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_STRING_SYMBOL_POOL_H
#define MCLD_STRING_SYMBOL_POOL_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/StringRef.h>
#include <mcld/ADT/HashTable.h>
#include <mcld/ADT/StringHash.h>
#include <mcld/ADT/Uncopyable.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/ResolveInfoFactory.h>
#include <utility>

namespace llvm
{
  class MCSectionData;
}

namespace mcld
{

class Resolver;
class StringTable;
class SymbolTableIF;

/** \class StrSymPool
 *  \brief Store symbol and search symbol by name. Can help symbol resolution.
 *
 *  - MCLinker is responsed for creating StrSymPool.
 */
class StrSymPool : private Uncopyable
{
public:
  typedef HashTable<ResolveInfo, StringHash<ELF>, ResolveInfoFactory> Table;
  typedef size_t size_type;

public:
  StrSymPool(const Resolver& pResolver, size_type pSize = 3);
  ~StrSymPool();

  // -----  modifiers  ----- //
  /// createSymbol - create a symbol but do not insert into the pool.
  ResolveInfo* createSymbol(const llvm::StringRef& pName,
                            bool pIsDyn,
                            ResolveInfo::Desc pDesc,
                            ResolveInfo::Binding pBinding,
                            ResolveInfo::SizeType pSize,
                            ResolveInfo::Visibility pVisibility = ResolveInfo::Default);
  
  /// insertSymbol - insert a symbol and resolve the symbol immediately
  /// @return the pointer to resolved ResolveInfo
  /// @return if the symbol is existent, return true. Otherwise, return false
  std::pair<ResolveInfo*, bool> insertSymbol(const llvm::StringRef& pName,
                                             bool pIsDyn,
                                             ResolveInfo::Desc pDesc,
                                             ResolveInfo::Binding pBinding,
                                             ResolveInfo::SizeType pSize,
                                             ResolveInfo::Visibility pVisibility =
                                                 ResolveInfo::Default);

  /// insertString - insert a string
  /// if the string has existed, modify pString to the existing string
  /// @return the StringRef points to the hash table
  llvm::StringRef insertString(const llvm::StringRef& pString);

  // -----  observers  ----- //
  size_type size() const
  { return m_Table.numOfEntries(); }

  bool empty() const
  { return m_Table.empty(); }

  // -----  capacity  ----- //
  void reserve(size_type pN);

  size_type capacity() const;

private:
  Resolver* m_pResolver;
  Table m_Table;

};

} // namespace of mcld

#endif

