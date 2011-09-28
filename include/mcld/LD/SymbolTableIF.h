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
 *  table of an input file has its own StrSymPool::SymbolCatagory. But the
 *  output's symbol table shares the StrSymPool::SymbolCatagory with
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
  typedef StrSymPool::CatagorySet          CatagorySet;
  typedef StrSymPool::SymbolCatagory       SymbolCatagory;
  typedef SymbolCatagory::iterator         iterator;
  typedef SymbolCatagory::const_iterator   const_iterator;

protected:
  SymbolTableIF(StrSymPool &pStrSymPool)
    : f_StrSymPool(pStrSymPool)
  {}

public:
  virtual ~SymbolTableIF() {}

  // -----  observers  ----- //
  size_t size() const
  { return f_pCatagorySet->at(CatagorySet::Entire).size(); }

  const LDSymbol *getSymbol(int pX) const
  { return f_pCatagorySet->at(CatagorySet::Entire)[pX]; }

  // -----  modifiers  ----- //
  LDSymbol *getSymbol(int pX)
  { return f_pCatagorySet->at(CatagorySet::Entire)[pX]; }

  LDSymbol *insertSymbol(const char *pSymName,
                         bool pIsDynamic,
                         LDSymbol::Type pSymbolType,
                         LDSymbol::Binding pSymbolBinding,
                         const llvm::MCSectionData * pSection)
  {
    LDSymbol *sym = f_StrSymPool.insertSymbol(pSymName,
                                              pIsDynamic,
                                              pSymbolType,
                                              pSymbolBinding,
                                              pSection);
    doInsertSymbol(sym);
    return sym;
  }

  void merge(const SymbolTableIF &pSymTab)
  {
    f_StrSymPool.merge(pSymTab.f_StrSymPool);
    doMerge(pSymTab);
  }

  // -----  iterators  ----- //
  template<size_t Catagory>
  iterator begin()
  { return f_pCatagorySet->at(Catagory).begin(); }

  template<size_t Catagory>
  const_iterator begin() const
  { return f_pCatagorySet->at(Catagory).begin(); }

  template<size_t Catagory>
  iterator end()
  { return f_pCatagorySet->at(Catagory).end(); }

  template<size_t Catagory>
  const_iterator end() const
  { return f_pCatagorySet->at(Catagory).end(); }

  iterator begin(size_t pCatagory)
  { return f_pCatagorySet->at(pCatagory).begin(); }

  const_iterator begin(size_t pCatagory) const
  { return f_pCatagorySet->at(pCatagory).begin(); }

  iterator end(size_t pCatagory)
  { return f_pCatagorySet->at(pCatagory).end(); }

  const_iterator end(size_t pCatagory) const
  { return f_pCatagorySet->at(pCatagory).end(); }

  void interpose(StringTableIF *pStrTab)
  { f_pCatagorySet->interpose(pStrTab); }

private:
  virtual void doInsertSymbol(LDSymbol *) = 0;
  virtual void doMerge(const SymbolTableIF &) = 0;

  void setCatagorySet(CatagorySet *pSymCatagorySet)
  { f_pCatagorySet = pSymCatagorySet; }

protected:
  StrSymPool &f_StrSymPool;
  CatagorySet *f_pCatagorySet;
};

} // namespace of mcld

#endif

