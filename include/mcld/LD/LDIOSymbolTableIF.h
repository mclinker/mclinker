/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDIOSYMBOLTABLEIF_H
#define LDIOSYMBOLTABLEIF_H
#include <mcld/LD/LDSymbolTalbeIF.h>
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
class LDIOSymbolTableIF : public LDSymbolTalbeIF
{
private:
  LDIOSymbolTableIF();
protected:
  vector<LDSymbol*> f_Symbols;
public:
  LDSymbol *getSymbol(int pX) const {
    return f_Symbols[pX];
  }
  virtual void insertSymbol(llvm::StringRef) {}
  virtual const_iterator begin() const {}
  virtual iterator begin() {}
  virtual const_iterator end() const {}
  virtual iterator end() {}
  virtual size_t size() const {
    return f_Symbols.size();
  }
  virtual bool merge(const LDSymbolTalbeIF*) {}
}

} // namespace of mcld

#endif

