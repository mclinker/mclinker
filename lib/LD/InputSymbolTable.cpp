/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/InputSymbolTable.h>
#include <vector>

using namespace mcld;

//==========================
// InputSymbolTable

InputSymbolTable::InputSymbolTable(StrSymPool &pStrSymPool, size_t reserve)
  : SymbolTableIF(pStrSymPool)
{
  f_StrSymPool.addIndirectClient(*this);
  f_pCatagorySet->at(CatagorySet::Entire).reserve(reserve);
}

void InputSymbolTable::doInsertSymbol(LDSymbol *pSym)
{
  f_pCatagorySet->insertSymbolPointer(pSym);
}

void InputSymbolTable::doMerge(const SymbolTableIF &pSymTab)
{
  if (this == &pSymTab)
     return;
  for (size_t i = 0; i < CatagorySet::NumOfCatagories; ++i)
    f_pCatagorySet->at(i).insert(f_pCatagorySet->at(i).end(),
                                 pSymTab.begin(i),
                                 pSymTab.end(i));
}

InputSymbolTable::~InputSymbolTable()
{
}
