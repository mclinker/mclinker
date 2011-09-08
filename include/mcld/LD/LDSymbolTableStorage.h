/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDSYMBOLTABLESTORAGE_H
#define LDSYMBOLTABLESTORAGE_H
#include <mcld/LD/StringTable.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class LDSymbolTableStorage
 *  \brief Store symbol and search symbol by name. Can help symbol resolution.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDSymbolTableStorage
{
  /* draft. */
  friend class LDSymbolTableFactory;
  LDSymbolTableStorage(StringTable *pStrTab):m_Strtab(pStrTab){}
public:
  typedef vector<LDSymbol *> SymbolList;
private:
  StringTable *m_Strtab;
  vector<LDSymbol *> m_SymbolList;
private:
  m_SymbolList *getSymbolList() {
    return &m_SymbolList;
  }
};

} // namespace of mcld

#endif

