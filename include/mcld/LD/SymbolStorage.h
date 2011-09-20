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
#include <mcld/LD/SymbolTableEntry.h>
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

template<class, class, class, class> class StringUnorderedMap;
class StringUnorderedMapDefaultHash;

/** \class SymbolStorage
 *  \brief Store symbol and search symbol by name. Can help symbol resolution.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class SymbolStorage : private Uncopyable
{
  /* draft. */
  friend class SymbolTableFactory;
  enum {
    NumOfEntries = 256
  };
  typedef SymbolTableEntry<ShouldOverwrite> SymbolTableEntryType;
  typedef StringUnorderedMap<SymbolTableEntryType *,
                             const char *,
                             StringUnorderedMapDefaultHash,
                             LinearAllocator<SymbolTableEntryType*,
                                             NumOfEntries> > SearcherType;


private:
  SymbolStorage(StringTableIF &pStrTab);

  // TODO: Release all allocated memory.
  ~SymbolStorage();

public:
  typedef std::vector<LDSymbol *>      SymbolList;

  LDSymbol *insertSymbol(llvm::StringRef,
                         bool,
                         LDSymbol::Type,
                         LDSymbol::Binding,
                         const llvm::MCSectionData *);
  void merge(const SymbolStorage &);

private:
  StringTableIF &m_StrTab;
  SymbolList m_EntireSymList;
  SymbolList m_DynamicSymList;
  SymbolList m_CommonSymList;
  GCFactory<SymbolTableEntryType, 0> m_EntryAllocator;
  LinearAllocator<LDSymbol, 0> m_SymbolAllocator;

private:
  SearcherType *m_pSymbolSearch;

private:
  SymbolList &entireSymbolList()
  { return m_EntireSymList; }

  SymbolList &dynamicSymbolList()
  { return m_DynamicSymList; }

  SymbolList &commonSymbolList()
  { return m_CommonSymList; }
};

} // namespace of mcld

#endif

