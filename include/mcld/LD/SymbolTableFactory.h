//===- SymbolTableFactory.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SYMBOL_TABLE_FACTORY_H
#define MCLD_SYMBOL_TABLE_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/LD/InputSymbolTable.h"
#include "mcld/LD/OutputSymbolTable.h"

namespace mcld
{

class StringTable;
class NamePool;

/** \class SymbolTableFactory
 *  \brief SymbolTableFactory manages SymbolTableIFs.
 *
 *  SymbolTableFactory is responsed for construction and destruction of 
 *  SymbolTableIF. Since different MCLDFiles have different type of 
 *  SymbolTableIF, SymbolTableFactory separates the construction of 
 *  SymbolTableIF into createInputTable() and createOutputTable().
 *
 *  @see SymbolTableIF InputSymbolTable OutputSymbolTable
 */
class SymbolTableFactory
{
public:
  /// SymbolTableFactory - constructor
  //  @param pNumOfSymbolTables is the most appropriate number of created 
  //  symbol tables.
  //  @param pStorage the real storage of created symbols
  explicit SymbolTableFactory(size_t pNumOfSymbolTables,
                              NamePool& pNamePool);
  /// ~SymbolTableFactory - destructor
  //  destructor destroys all created symbol tables.
  ~SymbolTableFactory();

  /// createInputTable - create a symbol table for an input file
  //  @param pEntireStringTable the string table of created Symbols.
  //  @param pDynamicStringTable the string table of created Dynamic Symbols.
  //  @param pReserve Created symbol table must reserve pReserve number of 
  //  storages of symbol for creating symbols.
  SymbolTableIF *createInputTable(StringTable &pEntireStringTable,
                                  StringTable &pDynamicStringTable,
                                  size_t pReserve=256);

  /// createOutputTable - create a symbol table for an output file
  //  @param pEntireStringTable the string table of created Symbols.
  //  @param pDynamicStringTable the string table of created Dynamic Symbols.
  //  @param pReserve Created symbol table must reserve pReserve number of 
  //  storages of symbol for creating symbols.
  SymbolTableIF *createOutputTable(StringTable &pEntireStringTable,
                                   StringTable &pDynamicStringTable,
                                   size_t pReserve=256);
private:
  NamePool &m_NamePool;
  GCFactory<InputSymbolTable, 0> m_InputFactory;
  GCFactory<OutputSymbolTable, 0> m_OutputFactory;

};

} // namespace of mcld

#endif

