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
public:
  typedef SymbolTableStrorage::iterator iterator;
  typedef SymbolTableStrorage::const_iterator const_iterator;
private:
  LDDirectSymbolTable(LDSymbolTableStorage *symtab):LDSymbolTableIF(symtab){}
public:
  const_iterator begin() const { return f_SymbolTableStrorage->begin(); }
  iterator begin() { return f_SymbolTableStrorage->begin(); }
  const_iterator end() const { return f_SymbolTableStrorage->end(); }
  iterator end() { return f_SymbolTableStrorage->end(); }
  virtual void insertSymbol(llvm::StringRef pSymName) {
    f_SymbolTableStrorage->insertSymbol(pSymName);
  }
  virtual size_t size() const { return f_SymbolTableStrorage->size(); }
  virtual bool merge(const LDSymbolTableIF *) {
    f_SymbolTableStrorage->merge();
  }
}

} // namespace of mcld

#endif

