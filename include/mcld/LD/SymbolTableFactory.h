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
  SymbolTableIF *create(StringTableIF*, const Input&, size_t reserve=256);
  SymbolTableIF *create(StringTableIF*, const Output&, size_t reserve=256);
};

template
class SymbolTableFactory<true>
{
  /* draft. */
public:
  SymbolTableFactory() {
  }
  SymbolTableIF *create(StringTableIF *pStrTab, const Input&, size_t reserve=256) {
    return new InputSymbolTable(SymbolTableFactory::Instance(pStrTab), reserve);
  }
  SymbolTableIF *create(StringTableIF *pStrTab, const Output&, size_t reserve=256) {
    SymbolStorage *symtab = SymbolTableFactory::Instance(pStrTab);
    return new OutputSymbolTable(symtab, symtab->getSymbolList(), reserve);
  }
private:
  SymbolStorage *instance(StringTableIF *pStrTab, size_t reserve) {
    static SymbolStorage *singleton = new SymbolStorage(pStrTab);
    return singleton;
  }
};

} // namespace of mcld

#endif

