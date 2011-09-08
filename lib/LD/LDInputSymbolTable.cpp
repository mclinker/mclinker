/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/LDInputSymbolTable.h>
#include <mcld/LD/LDSymbolTableStorage.h>

using namespace mcld;

//==========================
// LDInputSymbolTable

LDInputSymbolTable::LDInputSymbolTable(LDSymbolTableStorage *pSymTab)
  : LDSymbolTableIF(pSymTab)
{
  f_Symbols = new vector<LDSymbol *>;
}

virtual void LDInputSymbolTable::insertSymbol_impl(llvm::StringRef pSymName)
{
  f_Symbols->push_back(f_SymbolTableStrorage->insertSymbol(pSymName));
}

virtual void LDInputSymbolTable::merge_impl(const LDSymbolTableIF &pSymTab)
{
  if(this!=&pSymTab) {
    f_SymbolTableStrorage->merge(pSymTab.f_SymbolTableStrorage);
    f_Symbols->insert(f_Symbols->end(), pSymTab.begin(), pSymTab.end());
  }
}

virtual LDInputSymbolTable::~LDInputSymbolTable()
{
  delete f_Symbols;
}
