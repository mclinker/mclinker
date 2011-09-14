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
  SymbolStorage(StringTableIF *pStrTab):m_Strtab(pStrTab){}
public:
  typedef vector<LDSymbol *> SymbolList;
  void insertSymbol(llvm::StringRef);
  void merge(const SymbolStorage &);
private:
  StringTableIF *m_Strtab;
  SymbolList m_SymList;
  GCFactory<LDSymbol *, 256> m_Allocator;
private:
  m_SymbolList *getSymbolList() {
    return &m_SymbolList;
  }
};

} // namespace of mcld

#endif

