/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/StrSymPool.h>
#include <mcld/LD/SymbolTableIF.h>
#include <cassert>

using namespace std;

namespace mcld
{

//==========================
// StrSymPool

StrSymPool::StrSymPool(size_t pNumOfSymbols, 
                       size_t pNumOfInputs,
                       Resolver &pResolver)
  : m_EntryAllocator(pNumOfSymbols),
    m_SymbolAllocator(pNumOfSymbols),
    m_CatagorySetAllocator(pNumOfInputs),
    m_Resolver(pResolver)
{
}

LDSymbol *StrSymPool::insertSymbol(const char *pSymName,
                                   bool pDynamic,
                                   LDSymbol::Type pType,
                                   LDSymbol::Binding pBinding,
                                   const llvm::MCSectionData *pSection)
{
  SearcherType::value_type &mapEntry =
    m_SymbolSearch.GetOrCreateValue(pSymName);
  if (pBinding == LDSymbol::Local) {
    /* If the symbole is a local symbol, we don't care the same name symbol
     * already exists or not. Just allocate a new LDSymbol and return it.
     */
    LDSymbol *new_sym = m_SymbolAllocator.allocate(1);
    new ((void *)new_sym) LDSymbol();

    new_sym->setName(mapEntry.getKeyData());
    new_sym->setDynamic(pDynamic);
    new_sym->setType(pType);
    new_sym->setBinding(pBinding);
    new_sym->setSection(pSection);

    return new_sym;
  }
  else {
    /* If the symbole is not a local symbol, we should check that the same name
     * symbol already exists or not. If it already exists, we should use
     * resolver to decide which symbol we need to store.
     */
    EntryType *&symbolEntry = mapEntry.getValue();
    if (symbolEntry == 0) {
      symbolEntry = m_EntryAllocator.allocate();
      new (symbolEntry) EntryType();

      LDSymbol &symbol = symbolEntry->symbol();

      symbol.setName(mapEntry.getKeyData());
      symbol.setDynamic(pDynamic);
      symbol.setType(pType);
      symbol.setBinding(pBinding);
      symbol.setSection(pSection);
    }
    else {
      /* There is a same name symbol already exists. */
      LDSymbol new_sym;
      new_sym.setDynamic(pDynamic);
      new_sym.setType(pType);
      new_sym.setBinding(pBinding);
      new_sym.setSection(pSection);

      symbolEntry->addSameNameSymbol(new_sym, m_Resolver);
    }
    return &symbolEntry->symbol();
  }
}

const char *StrSymPool::insertString(const char *pSymName)
{
  return m_SymbolSearch.GetOrCreateValue(pSymName).getKeyData();
}

void StrSymPool::merge(const StrSymPool &pStrSymPool)
{
  if (this == &pStrSymPool)
    return;
  assert(0 && "Not implemented.");
}

void StrSymPool::addIndirectClient(SymbolTableIF &pSymTab)
{
  CatagorySet *catagorySet = m_CatagorySetAllocator.allocate(1);
  new (catagorySet) CatagorySet();
  pSymTab.setCatagorySet(catagorySet);
}

void StrSymPool::addDirectClient(SymbolTableIF &pSymTab)
{
  pSymTab.setCatagorySet(&m_CatagorySet);
}

} // namespace of mcld
