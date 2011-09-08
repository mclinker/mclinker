/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDOUTPUTSYMBOLTABLE_H
#define LDOUTPUTSYMBOLTABLE_H
#include <llvm/ADT/StringRef.h>
#include <mcld/LD/LDSymbolTableIF.h>
#include <mcld/LD/LDSymbolTableStorage.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;

/** \class LDOutputSymbolTable
 *  \brief Output symbol table, for MCLDOutput.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDOutputSymbolTable : public LDSymbolTableIF
{
  /* draft. */
friend class LDSymbolTableFactory;
private:
  LDOutputSymbolTable(LDSymbolTableStorage *pSymTab);
private:
  virtual void insertSymbol_impl(llvm::StringRef);
  virtual void merge_impl(const LDSymbolTableIF &);
} // namespace of mcld

#endif

