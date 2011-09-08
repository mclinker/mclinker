/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDDIRECTSYMBOLTABLE_H
#define LDDIRECTSYMBOLTABLE_H
#include <llvm/ADT/StringRef.h>
#include <mcld/LD/LDSymbolTableIF.h>
#include <mcld/LD/LDSymbolTableStorage.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;


/** \class LDDirectSymbolTable
 *  \brief For generally used symbol table.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDDirectSymbolTable : public LDSymbolTableIF
{
  /* draft. */
friend class LDSymbolTableFactory;
private:
  LDDirectSymbolTable(LDSymbolTableStorage *symtab):LDSymbolTableIF(symtab){}
public:
  typedef SymbolTableStrorage::iterator          iterator;
  typedef SymbolTableStrorage::const_iterator    const_iterator;
  const_iterator begin() const { return f_SymbolTableStrorage->begin(); }
  iterator begin() { return f_SymbolTableStrorage->begin(); }
  const_iterator end() const { return f_SymbolTableStrorage->end(); }
  iterator end() { return f_SymbolTableStrorage->end(); }
private:
  virtual void insertSymbol_impl(llvm::StringRef pSymName) {
    f_SymbolTableStrorage->insertSymbol(pSymName);
  }
  virtual size_t size_impl() const {
    return f_SymbolTableStrorage->size();
  }
  virtual void merge_impl(const LDSymbolTableIF &pSymTab) {
    f_SymbolTableStrorage.merge(pSymTab.f_SymbolTableStrorage);
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

