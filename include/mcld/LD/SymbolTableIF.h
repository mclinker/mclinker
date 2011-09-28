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
#include <mcld/LD/StrSymPool.h>
#include <vector>

namespace mcld
{

/** \class SymbolTableIF
 *  \brief SymbolTableIF is an abstract interfaoce of symbol tables
 *
 *  Although the operations on the symbol tables of the input and the output
 *  files are similar, their memory layouts are very different. Each symbol
 *  table of an input file has its own StrSymPool::SymbolCategory. But the
 *  output's symbol table shares the StrSymPool::SymbolCategory with
 *  StrSymPool.
 *
 *  SymbolTableIf is the common operations on both input's and output's 
 *  symbol tables.
 *
 *  @see StrSymPool
 */
class SymbolTableIF : private Uncopyable
{
  friend class StrSymPool;
public:
  typedef StrSymPool::CategorySet          CategorySet;
  typedef StrSymPool::SymbolCategory       SymbolCategory;
  typedef SymbolCategory::iterator         iterator;
  typedef SymbolCategory::const_iterator   const_iterator;

protected:
  SymbolTableIF(StrSymPool &pStrSymPool)
    : f_StrSymPool(pStrSymPool)
  {}

public:
  virtual ~SymbolTableIF() {}

  // -----  observers  ----- //
  size_t size() const
  { return f_pCategorySet->at(CategorySet::Entire).size(); }

  const LDSymbol *getSymbol(int pX) const
  { return f_pCategorySet->at(CategorySet::Entire)[pX]; }

  // -----  modifiers  ----- //
  LDSymbol *getSymbol(int pX)
  { return f_pCategorySet->at(CategorySet::Entire)[pX]; }

  LDSymbol *insertSymbol(const char *pSymName,
                         bool pIsDynamic,
                         LDSymbol::Type pSymbolType,
                         LDSymbol::Binding pSymbolBinding,
                         const llvm::MCSectionData * pSection,
                         uint64_t pValue,
                         uint64_t pSize,
                         uint8_t pOther)
  {
    LDSymbol *sym = f_StrSymPool.insertSymbol(pSymName,
                                              pIsDynamic,
                                              pSymbolType,
                                              pSymbolBinding,
                                              pSection,
                                              pValue,
                                              pSize,
                                              pOther);
    doInsertSymbol(sym);
    return sym;
  }

  void merge(const SymbolTableIF &pSymTab)
  {
    f_StrSymPool.merge(pSymTab.f_StrSymPool);
    doMerge(pSymTab);
  }

  // -----  iterators  ----- //
  template<size_t Category>
  iterator begin()
  { return f_pCategorySet->at(Category).begin(); }

  template<size_t Category>
  const_iterator begin() const
  { return f_pCategorySet->at(Category).begin(); }

  template<size_t Category>
  iterator end()
  { return f_pCategorySet->at(Category).end(); }

  template<size_t Category>
  const_iterator end() const
  { return f_pCategorySet->at(Category).end(); }

  iterator begin(size_t pCategory)
  { return f_pCategorySet->at(pCategory).begin(); }

  const_iterator begin(size_t pCategory) const
  { return f_pCategorySet->at(pCategory).begin(); }

  iterator end(size_t pCategory)
  { return f_pCategorySet->at(pCategory).end(); }

  const_iterator end(size_t pCategory) const
  { return f_pCategorySet->at(pCategory).end(); }

private:
  virtual void doInsertSymbol(LDSymbol *) = 0;
  virtual void doMerge(const SymbolTableIF &) = 0;

  void setCategorySet(CategorySet *pSymCategorySet)
  { f_pCategorySet = pSymCategorySet; }

protected:
  StrSymPool &f_StrSymPool;
  CategorySet *f_pCategorySet;
};

} // namespace of mcld

#endif

