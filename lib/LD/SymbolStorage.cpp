/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/SymbolStorage.h>
#include <mcld/LD/StringTableIF.h>
#include <mcld/LD/SymbolTableEntry.h>
#include <cassert>

using namespace std;

namespace mcld
{

// Partial specialization for insert string to string table when symbol is not
// in the symbol table. (If the symbol is already in symbol table, we don't
// need to insert the same string again, the string address is in the symbol
// which is already in symbol table.)
template<typename StringType>
struct StringUnorderedMapInit<SymbolStorage::SymbolTableEntryType *,
                              StringType>
{
  void operator()(StringType &pKey,
                  SymbolStorage::SymbolTableEntryType *&pValue,
                  llvm::StringRef pStr,
                  StringTableIF *pStrTab) {
    ::new ((void *)&pKey) StringType(pStrTab->insert(pStr));
    ::new ((void *)&pValue) typename SymbolStorage::SymbolTableEntryType *(0);
  }
};

//==========================
// SymbolStorage
// FIXME: set Symbol value.
LDSymbol *SymbolStorage::insertSymbol(llvm::StringRef pSymName)
{
  LDSymbol sym;
  // FIXME: don't set Symbol value right now.
  if(sym.binding() == LDSymbol::Local) {
    // FIXME: Check binding without LDSymbol.
    LDSymbol *new_sym = m_SymbolAllocator.allocate(1);
    ::new ((void *)new_sym) LDSymbol();
    // FIXME: set Symbol value.
  }
  else {
    SymbolTableEntryType *&entry =
      m_SymbolSearch.getOrCreate(pSymName, m_StrTab);
    if (entry) {
      entry->addSameNameSymbol(&sym);
    }
    else {
      LDSymbol *new_sym = m_SymbolAllocator.allocate(1);
      ::new (new_sym) LDSymbol();

      // FIXME: set Symbol value.
      entry = m_EntryAllocator.allocate(1);
      ::new (entry) SymbolTableEntryType(new_sym);

      m_SymList.push_back(new_sym);
    }
  }
}

void SymbolStorage::merge(const SymbolStorage &pSymTab)
{
  if(this==&pSymTab) return;
  assert(0 && "Not implemented.");
  /* FIXME TODO: Efficiently merge. NOTE: The string table may be different. */
}


} // namespace of mcld
