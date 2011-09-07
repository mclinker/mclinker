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
  LDDirectSymbolTable(StringTable *);
public:
  virtual void insertSymbol(llvm::StringRef) {}
  virtual const_iterator begin() const {}
  virtual iterator begin() {}
  virtual const_iterator end() const {}
  virtual iterator end() {}
  virtual bool merge(const LDSymbolTableIF*) {}
}

} // namespace of mcld

#endif

