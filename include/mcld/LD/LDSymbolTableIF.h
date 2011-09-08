/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDSYMBOLTABLEIF_H
#define LDSYMBOLTABLEIF_H
#include <llvm/ADT/StringRef.h>
#include <mcld/ADT/Uncopyable.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;
class LDSymbolTableStorage;

/** \class LDSymbolTableIF
 *  \brief Symbol table interface.
 *  Symbol table is not the symbol table strorage owner.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDSymbolTableIF : class Uncopyable
{
protected:
  LDSymbolTableIF(LDSymbolTableStorage *symtab):f_SymbolTableStrorage(symtab){}
  LDSymbolTableStorage *f_SymbolTableStrorage;
  vector<LDSymbol *> *f_Symbols;
public:
  LDSymbol *getSymbol(int pX) const { return (*f_Symbols)[pX]; }
  void insertSymbol(llvm::StringRef pSymName) { insertSymbol_impl(pSymName); }
  size_t size() const { return f_Symbols->size(); }
  void merge(const LDSymbolTableIF &pSymTab) { merge_impl(pSymTab); }
  virtual ~LDSymbolTableIF {}
public:
  typedef vector<LDSymbol *>::iterator         iterator;
  typedef vector<LDSymbol *>::const_iterator   const_terator;
  const_iterator begin() const { return f_Symbols->begin(); }
  iterator begin() const { return f_Symbols->begin(); }
  const_iterator end() const { return f_Symbols->begin(); }
  iterator end() const { return f_Symbols->end(); }
private:
  virtual void insertSymbol_impl(llvm::StringRef) = 0;
  virtual void merge_impl(const LDSymbolTableIF &) = 0;
}

} // namespace of mcld

#endif

