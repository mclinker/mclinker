/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/InputSymbolTable.h>
#include <mcld/LD/SymbolStorage.h>
#include <mcld/LD/LDSymbol.h>
#include <vector>

using namespace mcld;

//==========================
// InputSymbolTable

InputSymbolTable::InputSymbolTable(SymbolStorage &pSymStorage, size_t reserve)
  : SymbolTableIF(pSymStorage,
                  *new SymbolList(),
                  *new SymbolList(),
                  *new SymbolList())
{
  f_EntireSymList.reserve(reserve);
}

void InputSymbolTable::doInsertSymbol(LDSymbol *sym)
{
  f_EntireSymList.push_back(sym);
  if (sym->isDyn())
    f_DynamicSymList.push_back(sym);
  if (sym->type() == LDSymbol::Common)
    f_CommonSymList.push_back(sym);
}

void InputSymbolTable::doMerge(const SymbolTableIF &pSymTab)
{
  if (this == &pSymTab)
     return;
  f_EntireSymList.insert(f_EntireSymList.end(),
                         pSymTab.begin(),
                         pSymTab.end());
  f_DynamicSymList.insert(f_DynamicSymList.end(),
                          pSymTab.dyn_begin(),
                          pSymTab.dyn_end());
  f_CommonSymList.insert(f_DynamicSymList.end(),
                         pSymTab.com_begin(),
                         pSymTab.com_end());
}

InputSymbolTable::~InputSymbolTable()
{
  delete &f_EntireSymList;
  delete &f_DynamicSymList;
  delete &f_CommonSymList;
}
