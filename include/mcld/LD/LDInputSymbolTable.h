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
private:
  virtual void insertSymbol_impl(llvm::StringRef);
}

} // namespace of mcld

#endif

