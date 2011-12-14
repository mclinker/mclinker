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

#include "mcld/ADT/HashTable.h"
#include "mcld/ADT/StringHash.h"
#include "llvm/ADT/StringRef.h"
#include "mcld/ADT/Uncopyable.h"
#include "mcld/LD/ResolveInfo.h"
#include "mcld/LD/ResolveInfoFactory.h"

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
  /// insertSymbol - insert a symbol and resolve the symbol immediately
  /// @return if the table is changed, return true. Otherwise, return false
  bool insertSymbol(const llvm::StringRef& pName,
                    bool pIsDyn,
                    ResolveInfo::Type pType,
                    ResolveInfo::Binding pBinding,
                    ResolveInfo::ValueType pValue,
                    ResolveInfo::Visibility pVisibility = ResolveInfo::Default);

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

