/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/OutputSymbolTable.h>
#include <mcld/LD/SymbolStorage.h>

using namespace mcld;

//==========================
// OutputSymbolTable


OutputSymbolTable::OutputSymbolTable(SymbolStorage *pSymTab,
                                     SymbolList *pSymList,
                                     size_t reserve)
  : SymbolTableIF(pSymTab), f_Symbols(pSymList)
{
  f_Symbols->reserve(reserve);
}

virtual void OutputSymbolTable::doInsertSymbol(llvm::StringRef pSymName)
{
  f_SymbolTableStrorage->insertSymbol(pSymName);
}

virtual void OutputSymbolTable::doMerge(const SymbolTableIF &pSymTab)
{
  if(this==&pSymTab) return;
  f_SymbolTableStrorage->merge(*pSymTab.f_SymbolTableStrorage);
}
