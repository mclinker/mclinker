/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef SYMBOLSTORAGE_H
#define SYMBOLSTORAGE_H
#include <mcld/LD/StringTableIF.h>
#include <mcld/LD/SymbolTableEntry.h>
#include <mcld/LD/StringUnorderedMap.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class SymbolStorage
 *  \brief Store symbol and search symbol by name. Can help symbol resolution.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class SymbolStorage
{
  /* draft. */
  friend class SymbolTableFactory;
  SymbolStorage(StringTableIF *pStrTab):m_Strtab(pStrTab),m_Allocator(256){}
public:
  typedef vector<LDSymbol *> SymbolList;

  void insertSymbol(llvm::StringRef);
  void merge(const SymbolStorage &);
private:
  typedef SymbolTableEntry<ShouldOverwrite> SymbolTableEntryType;
  StringTableIF *m_Strtab;
  SymbolList m_SymList;
  GCFactory<SymbolTableEntryType> m_Allocator;

  template<typename DataType>
  class GCFactory256 : public GCFactory<DataType, 256> {};
  StringUnorderedMap<SymbolTableEntryType *,
                     const char *,
                     StringUnorderedMapDefaultHash,
                     GCFactory256> m_SymbolSearch;
private:
  m_SymbolList *getSymbolList() {
    return &m_SymbolList;
  }
};

} // namespace of mcld

#endif

