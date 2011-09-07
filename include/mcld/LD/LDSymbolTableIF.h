/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef LDSYMBOLTABLEIF_H
#define LDSYMBOLTABLEIF_H
#include <llvm/ADT/StringRef.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;
class LDSymbolTableStorage;


/** \class LDSymbolTableIF
 *  \brief Symbol table interface.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class LDSymbolTableIF
{
  /* draft. */
  /* For the speed issue, iterator move to the next level interfase. It may be
   * come back after speed test.(Because the bound may be the "insertSymbol".)
   */
protected:
  LDSymbolTableIF(LDSymbolTableStorage *symtab):f_SymbolTableStrorage(symtab){}
  LDSymbolTableStorage *f_SymbolTableStrorage;
public:
  virtual void insertSymbol(llvm::StringRef) {}
  virtual size_t size() const ;
  virtual bool merge(const LDSymbolTableIF*) {}
  virtual ~LDSymbolTableIF {}
}

} // namespace of mcld

#endif

