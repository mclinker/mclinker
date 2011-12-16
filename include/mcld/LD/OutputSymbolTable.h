//===- OutputSymbolTable.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef OUTPUTSYMBOLTABLE_H
#define OUTPUTSYMBOLTABLE_H
#include <llvm/ADT/StringRef.h>
#include "mcld/LD/SymbolTableIF.h"
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

