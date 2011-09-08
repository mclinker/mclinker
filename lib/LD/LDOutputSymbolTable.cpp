/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/LDOutputSymbolTable.h>
#include <mcld/LD/LDSymbolTableStorage.h>

using namespace mcld;

//==========================
// LDOutputSymbolTable

virtual void LDOutputSymbolTable::insertSymbol_impl(llvm::StringRef pSymName)
{
  f_SymbolTableStrorage->insertSymbol(pSymName);
}

virtual void LDOutputSymbolTable::merge_impl(const LDSymbolTableIF &pSymTab)
{
  if(this!=&pSymTab) {
    f_SymbolTableStrorage->merge(pSymTab.f_SymbolTableStrorage);
  }
}
