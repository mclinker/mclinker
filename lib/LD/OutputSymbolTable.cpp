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


OutputSymbolTable::OutputSymbolTable(SymbolStorage &pSymStorage,
                                     SymbolList &pEntireSymList,
                                     SymbolList &pDynamicSymList,
                                     SymbolList &pCommonSymList,
                                     size_t reserve)
  : SymbolTableIF(pSymStorage,
                  pEntireSymList,
                  pDynamicSymList,
                  pCommonSymList)
{
  f_EntireSymList.reserve(reserve);
}

void OutputSymbolTable::doInsertSymbol(LDSymbol *sym)
{
  // OutputSymbolTable didn't have any real containers,
  // so no need to do anything.
}

void OutputSymbolTable::doMerge(const SymbolTableIF &pSymTab)
{
  // OutputSymbolTable didn't have any real containers,
  // so no need to do anything.
}
