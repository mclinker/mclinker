//===- InputSymbolTable.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/InputSymbolTable.h"
#include <vector>

using namespace mcld;

//==========================
// InputSymbolTable

InputSymbolTable::InputSymbolTable(StrSymPool &pStrSymPool,
                                   size_t pNumOfSymbols,
                                   StringTable &pEntireStringTable,
                                   StringTable &pDynamicStringTable)
  : SymbolTableIF(pStrSymPool)
{
  f_StrSymPool.addIndirectClient(*this);

  f_pCategorySet->at(CategorySet::Entire).reserve(pNumOfSymbols);

  f_pCategorySet->at(CategorySet::Entire).interpose(&pEntireStringTable);
  f_pCategorySet->at(CategorySet::Dynamic).interpose(&pDynamicStringTable);
}

void InputSymbolTable::doInsertSymbol(LDSymbol *pSym)
{
  f_pCategorySet->insertSymbolPointer(pSym);
}

void InputSymbolTable::doMerge(const SymbolTableIF &pSymTab)
{
  if (this == &pSymTab)
     return;
  for (size_t i = 0; i < CategorySet::NumOfCategories; ++i)
    f_pCategorySet->at(i).insert(f_pCategorySet->at(i).end(),
                                 pSymTab.begin(i),
                                 pSymTab.end(i));
}

InputSymbolTable::~InputSymbolTable()
{
}
