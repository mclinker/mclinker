/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/InputSymbolTable.h>
#include <mcld/LD/SymbolStorage.h>

using namespace mcld;

//==========================
// InputSymbolTable

InputSymbolTable::InputSymbolTable(SymbolStorage *pSymTab, size_t reserve)
  : SymbolTableIF(pSymTab)
{
  f_Symbols = new vector<LDSymbol *>;
  f_Symbols->reserve(reserve);
}

virtual void InputSymbolTable::doInsertSymbol(llvm::StringRef pSymName)
{
  f_Symbols->push_back(f_SymbolTableStrorage->insertSymbol(pSymName));
}

virtual void InputSymbolTable::doMerge(const SymbolTableIF &pSymTab)
{
  if(this==&pSymTab) return;
  f_SymbolTableStrorage->merge(*pSymTab.f_SymbolTableStrorage);
  f_Symbols->insert(f_Symbols->end(), pSymTab.begin(), pSymTab.end());
}

virtual InputSymbolTable::~InputSymbolTable()
{
  delete f_Symbols;
}
