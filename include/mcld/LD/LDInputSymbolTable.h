/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDINPUTSYMBOLTABLE_H
#define LDINPUTSYMBOLTABLE_H
#include <llvm/ADT/StringRef.h>
#include <mcld/LD/LDIOSymbolTableIF.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;

/** \class LDInputSymbolTable
 *  \brief Input symbol table, for MCLDInput.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDInputSymbolTable : public LDIOSymbolTableIF
{
  /* draft. */
friend class LDSymbolTableFactory;
private:
  LDInputSymbolTable(LDSymbolTableStorage *symtab):LDIOSymbolTableIF(symtab){}
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

