/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef MCLD_SYMBOL_TABLE_FACTORY_H
#define MCLD_SYMBOL_TABLE_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/LD/InputSymbolTable.h>
#include <mcld/LD/OutputSymbolTable.h>
#include <mcld/LD/SymbolStorage.h>

namespace mcld
{

class StringTableIF;
class Input;
class Output;

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
                              SymbolStorage& pStorage);
  /// ~SymbolTableFactory - destructor
  //  destructor destroys all created symbol tables.
  ~SymbolTableFactory();

  /// createInputTable - create a symbol table for an input file
  //  @param pStrTab the string table of created Symbols.
  //  @param pReserve Created symbol table must reserve pReserve number of 
  //  storages of symbol for creating symbols.
  SymbolTableIF *createInputTable(StringTableIF &pStrTab,
                                  size_t pReserve=256);

  /// createOutputTable - create a symbol table for an output file
  //  @param pStrTab the string table of created Symbols.
  //  @param pReserve Created symbol table must reserve pReserve number of 
  //  storages of symbol for creating symbols.
  SymbolTableIF *createOutputTable(StringTableIF &pStrTab,
                                   size_t pReserve=256);
private:
  SymbolTableFactory &m_Storage;
  GCFactory<InputSymbolTable, 0> m_InputFactory;
  GCFactory<OutputSymbolTable, 0> m_OutputFactory;

};

} // namespace of mcld

#endif

