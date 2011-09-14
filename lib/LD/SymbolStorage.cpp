/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/SymbolStorage.h>
#include <mcld/LD/StringTableIF.h>
#include <cassert>

using namespace mcld;
using namespace std;

//==========================
// SymbolStorage

void SymbolStorage::insertSymbol(llvm::StringRef pSymName)
{
  /* TODO: insert. */
}
void SymbolStorage::merge(const SymbolStorage &pSymTab)
{
  if(this==&pSymTab) return;
  assert(0 && "Not implemented.");
  /* TODO: Efficiently merge. NOTE: The string table may be different. */
}
