/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef SYMBOLTABLEFACTORY_H
#define SYMBOLTABLEFACTORY_H
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/LD/SymbolStorage.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class SymbolTableFactory
 *  \brief SymbolTableFactory constructs the SymbolTables.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
template<bool global>
class SymbolTableFactory
{
  /* draft. */
public:
  SymbolTableFactory();
  SymbolTableIF *Create(StringTable *, const Input&);
  SymbolTableIF *Create(StringTable *, const Output&);
};

template
class SymbolTableFactory<true>
{
  /* draft. */
public:
  SymbolTableFactory() {
  }
  SymbolTableIF *Create(StringTable *pStrTab, const Input&) {
    return new InputSymbolTable(SymbolTableFactory::Instance(pStrTab));
  }
  SymbolTableIF *Create(StringTable *pStrTab, const Output&) {
    SymbolStorage *symtab = SymbolTableFactory::Instance(pStrTab);
    return new OutputSymbolTable(symtab, symtab->getSymbolList());
  }
private:
  SymbolStorage *Instance(StringTable *pStrTab) {
    static SymbolStorage *singleton = new SymbolStorage(pStrTab);
    return singleton;
  }
};

} // namespace of mcld

#endif

