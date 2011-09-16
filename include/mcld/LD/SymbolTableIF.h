/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef MCLD_SYMBOL_TABLE_INTERFACE_H
#define MCLD_SYMBOL_TABLE_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/StringRef.h>
#include <mcld/ADT/Uncopyable.h>

namespace mcld
{

class LDSymbol;

/** \class SymbolTableIF
 *  \brief SymbolTableIF is an abstract interfaoce of symbol tables
 *
 *  Although the operations on the symbol tables of the input and the output
 *  files are similar, their memory layouts are very different. Each symbol
 *  table of an input file has its own SymbolStorage::SymbolList. But the
 *  output's symbol table shares the SymbolStorage::SymbolList with
 *  SymbolStorage.
 *
 *  SymbolTableIf is the common operations on both input's and output's 
 *  symbol tables.
 *
 *  @see SymbolStorage
 */
class SymbolTableIF : private Uncopyable
{
protected:
  typedef SymbolStorage::SymbolList SymbolList;

public:
  typedef SymbolList::iterator         iterator;
  typedef SymbolList::const_iterator   const_terator;

protected:
  SymbolTableIF(SymbolStorage *pSymTab)
    : f_pSymbolTableStrorage(pSymTab), f_pSymbols(0) {}

public:
  virtual ~SymbolTableIF() {}

  // -----  observers  ----- //
  size_t size() const
  { return f_pSymbols->size(); }

  const LDSymbol *getSymbol(int pX) const
  { return (*f_pSymbols)[pX]; }

  // -----  modifiers  ----- //
  LDSymbol *getSymbol(int pX)
  { return (*f_pSymbols)[pX]; }

  void insertSymbol(llvm::StringRef pSymName)
  { doInsertSymbol(pSymName); }

  void merge(const SymbolTableIF &pSymTab)
  { doMerge(pSymTab); }

  // -----  iterators  ----- //
  const_iterator begin() const
  { return f_pSymbols->begin(); }

  iterator begin() const
  { return f_pSymbols->begin(); }

  const_iterator end() const
  { return f_pSymbols->begin(); }

  iterator end() const
  { return f_pSymbols->end(); }

private:
  virtual void doInsertSymbol(llvm::StringRef) = 0;
  virtual void doMerge(const SymbolTableIF &) = 0;

protected:
  SymbolStorage *f_pSymbolTableStrorage;
  SymbolList *f_pSymbols;
};

} // namespace of mcld

#endif

