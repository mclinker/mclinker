/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/LDOutputSymbolTable.h>
#include <mcld/LD/LDSymbolTableStorage.h>

using namespace mcld;

//==========================
// LDOutputSymbolTable

virtual void LDOutputSymbolTable::insertSymbol_impl(llvm::StringRef pSymName)
{
  f_Symbols.push_back(f_SymbolTableStrorage->insertSymbol(pSymName));
}
