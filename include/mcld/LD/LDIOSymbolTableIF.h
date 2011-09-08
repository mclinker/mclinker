/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDIOSYMBOLTABLEIF_H
#define LDIOSYMBOLTABLEIF_H
#include <llvm/ADT/StringRef.h>
#include <mcld/LD/LDSymbolTableIF.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;

/** \class LDIOSymbolTableIF
 *  \brief IO symbol table interface, for InputSymbolTable and OutputSymbolTable.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDIOSymbolTableIF : public LDSymbolTableIF
{
  /* draft. */
protected:
  LDIOSymbolTable(LDSymbolTableStorage *symtab):LDSymbolTableIF(symtab){}
  vector<LDSymbol*> f_Symbols;
public:
  LDSymbol *getSymbol(int pX) const {
    return f_Symbols[pX];
  }
public:
  typedef vector<LDSymbol*>::iterator         iterator;
  typedef vector<LDSymbol*>::const_iterator   const_iterator;
  const_iterator begin() const { return f_Symbols.begin(); }
  iterator begin() { return f_Symbols.begin(); }
  const_iterator end() const { return f_Symbols.end(); }
  iterator end() { return f_Symbols.end(); }
private:
  virtual void insertSymbol_impl(llvm::StringRef) {}
  virtual size_t size_impl() const {
    return f_Symbols.size();
  }
  virtual bool merge_impl(const LDSymbolTableIF &pSymTab) {
    if(this!=&pSymTab) {
      f_SymbolTableStrorage->merge(pSymTab.f_SymbolTableStrorage);
      f_Symbols.insert(f_Symbols.end(), pSymTab.begin(), pSymTab.end());
    }
  }
private:
  /* Iterator operation. */
  typedef LDSymbolTableIF::iterator          iterator_base;
  typedef LDSymbolTableIF::const_iterator    const_iterator_base;
  /* FIXME: Implement iterator to iterator_base. */
  virtual const_iterator_base begin_impl() const { return begin(); }
  virtual iterator_base begin_impl() { return begin(); }
  virtual const_iterator_base end_impl() const { return end(); }
  virtual iterator_base end_impl() { return end(); }
}

} // namespace of mcld

#endif

