/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/SymbolTableFactory.h>

using namespace mcld;

//==========================
// SymbolTableFactory

SymbolTableFactory::SymbolTableFactory(size_t pNumOfSymbolTables,
                                       StrSymPool& pStrSymPool)
  : m_StrSymPool(pStrSymPool),
    m_InputFactory(pNumOfSymbolTables)
    m_OutputFactory(pNumOfSymbolTables)
{
}

SymbolTableFactory::~SymbolTableFactory()
{
}

SymbolTableIF *SymbolTableFactory::
createInputTable(StringTableIF &pEntireStringTable,
                 StringTableIF &pDynamicStringTable,
                 size_t pReserve=256)
{
  InputSymbolTable inputSymTab = m_InputFactory.allocate();
  new (inputSymTab) InputSymbolTable(m_StrSymPool,
                                     pReserve,
                                     pEntireStringTable,
                                     pDynamicStringTable);
  return inputSymTab;
}

SymbolTableIF *SymbolTableFactory::
createOutputTable(StringTableIF &pEntireStringTable,
                  StringTableIF &pDynamicStringTable,
                  size_t pReserve=256)
{
  OutputSymbolTable outputSymTab = m_OutputFactory.allocate();
  new (outputSymTab) OutputSymbolTable(m_StrSymPool,
                                       pReserve,
                                       pEntireStringTable,
                                       pDynamicStringTable);
  return outputSymTab;
}
