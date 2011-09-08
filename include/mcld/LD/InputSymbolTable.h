/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef INPUTSYMBOLTABLE_H
#define INPUTSYMBOLTABLE_H
#include <llvm/ADT/StringRef.h>
#include <mcld/LD/SymbolTableIF.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;

/** \class InputSymbolTable
 *  \brief Input symbol table, for MCLDInput.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class InputSymbolTable : public SymbolTableIF
{
  /* draft. */
friend class SymbolTableFactory;
private:
  InputSymbolTable(SymbolStorage *pSymTab);
private:
  virtual void insertSymbol_impl(llvm::StringRef);
  virtual void merge_impl(const SymbolTableIF &);
public:
  virtual ~InputSymbolTable();
}

} // namespace of mcld

#endif

