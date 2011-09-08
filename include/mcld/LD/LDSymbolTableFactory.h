/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDSYMBOLTABLEFACTORY_H
#define LDSYMBOLTABLEFACTORY_H
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDOutput.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class LDSymbolTableFactory
 *  \brief LDSymbolTableFactory constructs the SymbolTables.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
template<bool global>
class LDSymbolTableFactory
{
  /* draft. */
public:
  LDSymbolTableFactory();
  LDIOSymbolTableIF *Create(StringTable *, const Input&);
  LDIOSymbolTableIF *Create(StringTable *, const Output&);
};

template
class LDSymbolTableFactory<true>
{
  /* draft. */
public:
  LDSymbolTableFactory() {
  }
  LDIOSymbolTableIF *Create(StringTable *pStrTab, const Input&) {
    return new LDInputSymbolTable(LDSymbolTableFactory::Instance(pStrTab));
  }
  LDIOSymbolTableIF *Create(StringTable *pStrTab, const Output&) {
    LDSymbolTableStorage *symtab = LDSymbolTableFactory::Instance(pStrTab);
    return new LDOutputSymbolTable(symtab, symtab->getSymbolList());
  }
private:
  LDSymbolTableStorage *Instance(StringTable *pStrTab) {
    static LDSymbolTableStorage *singleton = new LDSymbolTableStorage(pStrTab);
    return singleton;
  }
};

} // namespace of mcld

#endif

