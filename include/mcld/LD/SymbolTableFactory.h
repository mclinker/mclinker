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
#include <mcld/LD/SymbolTableIF.h>
#include <mcld/LD/InputSymbolTable.h>
#include <mcld/LD/OutputSymbolTable.h>
#include <mcld/LD/SymbolStorage.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class StringTableIF;

/** \class SymbolTableFactory
 *  \brief SymbolTableFactory constructs the SymbolTables.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class SymbolTableFactory
{
  /* draft. */
  /* FIXME: Who should delete symbol table. */
public:
  SymbolTableFactory() {
  }
  SymbolTableIF *create(StringTableIF &pStrTab,
                        const Input&,
                        size_t reserve=256) {
    SymbolStorage &symtab = SymbolTableFactory::instance(pStrTab);
    return new InputSymbolTable(symtab, reserve);
  }
  SymbolTableIF *create(StringTableIF &pStrTab,
                        const Output&,
                        size_t reserve=256) {
    SymbolStorage &symtab = SymbolTableFactory::instance(pStrTab);
    return new OutputSymbolTable(symtab,
                                 symtab.entireSymbolList(),
                                 symtab.dynamicSymbolList(),
                                 symtab.commonSymbolList(),
                                 reserve);
  }
private:
  static SymbolStorage &instance(StringTableIF &pStrTab) {
    static SymbolStorage singleton(pStrTab);
    return singleton;
  }
};

} // namespace of mcld

#endif

