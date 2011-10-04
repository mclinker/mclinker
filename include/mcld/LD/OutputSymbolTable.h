/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef OUTPUTSYMBOLTABLE_H
#define OUTPUTSYMBOLTABLE_H
#include <llvm/ADT/StringRef.h>
#include <mcld/LD/SymbolTableIF.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDSymbol;

/** \class OutputSymbolTable
 *  \brief Output symbol table, for MCLDOutput.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class OutputSymbolTable : public SymbolTableIF
{
  /* draft. */
  friend class SymbolTableFactory;
private:
  OutputSymbolTable(StrSymPool &pStrSymPool,
                    size_t pNumOfSymbols,
                    StringTable &pEntireStringTable,
                    StringTable &pDynamicStringTable);
private:
  virtual void doInsertSymbol(LDSymbol *);
  virtual void doMerge(const SymbolTableIF &);
};

} // namespace of mcld

#endif

