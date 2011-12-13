//===- SymbolTableFactory.cpp ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/SymbolTableFactory.h"

using namespace mcld;

//==========================
// SymbolTableFactory

SymbolTableFactory::SymbolTableFactory(size_t pNumOfSymbolTables,
                                       StrSymPool& pStrSymPool)
  : m_StrSymPool(pStrSymPool),
    m_InputFactory(pNumOfSymbolTables),
    m_OutputFactory(pNumOfSymbolTables)
{
}

SymbolTableFactory::~SymbolTableFactory()
{
}

SymbolTableIF *SymbolTableFactory::
createInputTable(StringTable &pEntireStringTable,
                 StringTable &pDynamicStringTable,
                 size_t pReserve)
{
  InputSymbolTable *inputSymTab = m_InputFactory.allocate();
  new (inputSymTab) InputSymbolTable(m_StrSymPool,
                                     pReserve,
                                     pEntireStringTable,
                                     pDynamicStringTable);
  return inputSymTab;
}

SymbolTableIF *SymbolTableFactory::
createOutputTable(StringTable &pEntireStringTable,
                  StringTable &pDynamicStringTable,
                  size_t pReserve)
{
  OutputSymbolTable *outputSymTab = m_OutputFactory.allocate();
  new (outputSymTab) OutputSymbolTable(m_StrSymPool,
                                       pReserve,
                                       pEntireStringTable,
                                       pDynamicStringTable);
  return outputSymTab;
}
