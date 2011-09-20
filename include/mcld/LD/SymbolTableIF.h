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
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/SymbolStorage.h>
#include <vector>

namespace mcld
{

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
public:
  typedef SymbolStorage::SymbolList    SymbolList;
  typedef SymbolList::iterator         iterator;
  typedef SymbolList::const_iterator   const_iterator;

protected:
  SymbolTableIF(SymbolStorage &pSymStorage,
                SymbolList &pEntireSymList,
                SymbolList &pDynamicSymList,
                SymbolList &pCommonSymList)
    : f_SymStorage(pSymStorage),
      f_EntireSymList(pEntireSymList),
      f_DynamicSymList(pDynamicSymList),
      f_CommonSymList(pCommonSymList)
  {}

public:
  virtual ~SymbolTableIF() {}

  // -----  observers  ----- //
  size_t size() const
  { return f_EntireSymList.size(); }

  const LDSymbol *getSymbol(int pX) const
  { return f_EntireSymList[pX]; }

  // -----  modifiers  ----- //
  LDSymbol *getSymbol(int pX)
  { return f_EntireSymList[pX]; }

  LDSymbol *insertSymbol(llvm::StringRef pSymName,
                         bool pDynamic,
                         LDSymbol::Type pType,
                         LDSymbol::Binding pBinding,
                         const llvm::MCSectionData* pSection)
  {
    LDSymbol *sym = f_SymStorage.insertSymbol(pSymName,
                                              pDynamic,
                                              pType,
                                              pBinding,
                                              pSection);
    doInsertSymbol(sym);
    return sym;
  }

  void merge(const SymbolTableIF &pSymTab)
  {
    f_SymStorage.merge(pSymTab.f_SymStorage);
    doMerge(pSymTab);
  }

  // -----  iterators  ----- //
  const_iterator begin() const
  { return f_EntireSymList.begin(); }

  iterator begin()
  { return f_EntireSymList.begin(); }

  const_iterator end() const
  { return f_EntireSymList.begin(); }

  iterator end()
  { return f_EntireSymList.end(); }

  const_iterator dyn_begin() const
  { return f_DynamicSymList.begin(); }

  iterator dyn_begin()
  { return f_DynamicSymList.begin(); }

  const_iterator dyn_end() const
  { return f_DynamicSymList.begin(); }

  iterator dyn_end()
  { return f_DynamicSymList.end(); }

  const_iterator com_begin() const
  { return f_CommonSymList.begin(); }

  iterator com_begin()
  { return f_CommonSymList.begin(); }

  const_iterator com_end() const
  { return f_CommonSymList.begin(); }

  iterator com_end()
  { return f_CommonSymList.end(); }

private:
  virtual void doInsertSymbol(LDSymbol *) = 0;
  virtual void doMerge(const SymbolTableIF &) = 0;

protected:
  SymbolStorage &f_SymStorage;
  SymbolList &f_EntireSymList;
  SymbolList &f_DynamicSymList;
  SymbolList &f_CommonSymList;
};

} // namespace of mcld

#endif

