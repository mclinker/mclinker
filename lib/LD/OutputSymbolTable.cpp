/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/OutputSymbolTable.h>

using namespace mcld;

//==========================
// OutputSymbolTable


OutputSymbolTable::OutputSymbolTable(StrSymPool &pStrSymPool,
                                     size_t pNumOfSymbols,
                                     StringTableIF &pEntireStringTable,
                                     StringTableIF &pDynamicStringTable)
  : SymbolTableIF(pStrSymPool)
{
  f_StrSymPool.addDirectClient(*this);

  f_pCategorySet->at(CategorySet::Entire).reserve(pNumOfSymbols);

  f_pCategorySet->at(CategorySet::Entire).interpose(&pEntireStringTable);
  f_pCategorySet->at(CategorySet::Dynamic).interpose(&pDynamicStringTable);
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
