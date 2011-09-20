/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef MCLD_SYMBOL_STORAGE_H
#define MCLD_SYMBOL_STORAGE_H
#include <mcld/ADT/Uncopyable.h>
#include <mcld/Support/Allocators.h>
#include <mcld/LD/StringTableIF.h>
#include <mcld/LD/SymbolTableIF.h>
#include <mcld/LD/SymbolTableEntry.h>
#include <mcld/LD/StringUnorderedMap.h>
#include <mcld/LD/LDSymbol.h>
#include <vector>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

struct ShouldOverwrite
{
  bool operator()(const LDSymbol *sym1, const LDSymbol *sym2) {
    return false;
  }
};

/** \class SymbolStorage
 *  \brief Store symbol and search symbol by name. Can help symbol resolution.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class SymbolStorage : private Uncopyable
{
  /* draft. */
  template<bool>
  friend class SymbolTableFactory;

  SymbolStorage(StringTableIF *pStrTab)
  : m_StrTab(pStrTab), m_EntryAllocator(256), m_SymbolAllocator(256)
  {}

  // TODO: Release all allocated memory.
  ~SymbolStorage()
  {}

public:
  typedef SymbolTableIF::SymbolList SymbolList;
  typedef SymbolTableEntry<ShouldOverwrite> SymbolTableEntryType;

  LDSymbol *insertSymbol(llvm::StringRef);
  void merge(const SymbolStorage &);

private:
  StringTableIF *m_StrTab;
  SymbolList m_SymList;
  GCFactory<SymbolTableEntryType, 0> m_EntryAllocator;
  LinearAllocator<LDSymbol, 0> m_SymbolAllocator;

  template<typename DataType>
  class GCFactory256 : public GCFactory<DataType, 256>
  {};

private:
  StringUnorderedMap<SymbolTableEntryType *,
                     const char *,
                     StringUnorderedMapDefaultHash,
                     GCFactory256> m_SymbolSearch;
private:
  SymbolList *getSymbolList() {
    return &m_SymList;
  }
};

} // namespace of mcld

#endif

