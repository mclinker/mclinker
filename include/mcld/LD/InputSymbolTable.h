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
  InputSymbolTable(StrSymPool &pStrSymPool,
                   size_t pNumOfSymbols,
                   StringTable &pEntireStringTable,
                   StringTable &pDynamicStringTable);
private:
  virtual void doInsertSymbol(LDSymbol *);
  virtual void doMerge(const SymbolTableIF &);
public:
  virtual ~InputSymbolTable();
};

} // namespace of mcld

#endif

