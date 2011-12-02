//===- StrSymPool.h -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
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
#include "mcld/ADT/Uncopyable.h"
#include "mcld/LD/Resolver.h"
#include "mcld/LD/ResolveInfo.h"
#include "mcld/LD/ResolveInfoFactory.h"

namespace llvm
{
  class MCSectionData;
}

namespace mcld
{

class StringTable;
class SymbolTableIF;

/** \class StrSymPool
 *  \brief Store symbol and search symbol by name. Can help symbol resolution.
 *
 *  - SectLinker is responsed for creating StrSymPool.
 */
class StrSymPool : private Uncopyable
{
public:
  typedef HashTable<ResolveInfo, StringHash<ELF>, ResolveInfoFactory> Table;
  typedef size_t size_type;

public:
  StrSymPool(size_type pSize = 3);
  ~StrSymPool();

  // -----  symbol resolution  ----- //

  // -----  modifiers  ----- //

  // -----  observers  ----- //
  size_type size() const
  { return m_Table.numOfEntries(); }

  bool empty() const
  { return m_Table.empty(); }

  // -----  capacity  ----- //
  void reserve(size_type pN);

  size_type capacity() const;

private:
  Resolver m_Resolver;
  Table m_Table;

};

} // namespace of mcld

#endif

