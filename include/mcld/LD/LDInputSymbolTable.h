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
#include <mcld/LD/LDSymbolTableIF.h>
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
class LDInputSymbolTable : public LDSymbolTableIF
{
  /* draft. */
friend class LDSymbolTableFactory;
private:
  LDInputSymbolTable(LDSymbolTableStorage *pSymTab):LDIOSymbolTableIF(pSymTab){}
private:
  virtual void insertSymbol_impl(llvm::StringRef);
  virtual void merge_impl(const LDSymbolTableIF &);
}

} // namespace of mcld

#endif

