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
  typedef vector<LDSymbol*>::iterator iterator;
  typedef vector<LDSymbol*>::const_iterator const_iterator;
public:
  LDSymbol *getSymbol(int pX) const {
    return f_Symbols[pX];
  }
  const_iterator begin() const { return f_Symbols.begin(); }
  iterator begin() { return f_Symbols.begin(); }
  const_iterator end() const { return f_Symbols.end(); }
  iterator end() { return f_Symbols.end(); }
  virtual void insertSymbol(llvm::StringRef) {}
  virtual size_t size() const { return f_Symbols.size(); }
  virtual bool merge(const LDSymbolTableIF *) {}
}

} // namespace of mcld

#endif

