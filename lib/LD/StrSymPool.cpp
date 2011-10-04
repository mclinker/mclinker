/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/LD/StrSymPool.h>
#include <mcld/LD/SymbolTableIF.h>
#include <mcld/LD/StringTable.h>
#include <cassert>

using namespace std;

namespace mcld
{

//==========================
// SymbolCategorySet::SymbolCategory

void SymbolCategorySet::SymbolCategory::push_back(const value_type &pVal)
{
  m_Symbols.push_back(pVal);
  if (m_Strtab)
    m_Strtab->insertExisted(pVal->name());
}


//==========================
// StrSymPool

StrSymPool::StrSymPool(size_t pNumOfSymbols, 
                       size_t pNumOfInputs,
                       Resolver &pResolver)
  : m_EntryAllocator(pNumOfSymbols),
    m_SymbolAllocator(pNumOfSymbols),
    m_CategorySetAllocator(pNumOfInputs),
    m_Resolver(pResolver)
{
}

LDSymbol *StrSymPool::insertSymbol(const char *pSymName,
                                   bool pDynamic,
                                   LDSymbol::Type pType,
                                   LDSymbol::Binding pBinding,
                                   const llvm::MCSectionData *pSection,
                                   uint64_t pValue,
                                   uint64_t pSize,
                                   uint8_t pOther)
{
  SearcherType::value_type &mapEntry =
    m_SymbolSearch.GetOrCreateValue(pSymName);
  LDSymbol new_sym;
  new_sym.setName(mapEntry.getKeyData());
  new_sym.setDynamic(pDynamic);
  new_sym.setType(pType);
  new_sym.setBinding(pBinding);
  new_sym.setSection(pSection);
  new_sym.setValue(pValue);
  new_sym.setSize(pSize);
  new_sym.setOther(pOther);
  if (pBinding == LDSymbol::Local) {
    /* If the symbole is a local symbol, we don't care the same name symbol
     * already exists or not. Just allocate a new LDSymbol and return it.
     */
    LDSymbol *new_sym_space = m_SymbolAllocator.allocate(1);
    new ((void *)new_sym_space) LDSymbol(new_sym);
    return new_sym_space;
  }
  else {
    /* If the symbole is not a local symbol, we should check that the same name
     * symbol already exists or not. If it already exists, we should use
     * resolver to decide which symbol we need to store.
     */
    EntryType *&symbolEntry = mapEntry.getValue();
    if (symbolEntry == 0) {
      symbolEntry = m_EntryAllocator.allocate();
      new (symbolEntry) EntryType(new_sym);
      m_CategorySet.insertSymbolPointer(&symbolEntry->symbol());
    }
    else {
      /* There is a same name symbol already exists. */
      if(!m_Resolver.shouldOverwrite(symbolEntry->symbol(), new_sym)) {
        symbolEntry->addReferenceSection(pSection);
      }
      else {
        /* should overwrite.*/
        m_CategorySet.moveSymbolToNewCategory(&symbolEntry->symbol(), new_sym);
        symbolEntry->replaceSymbol(new_sym);
      }

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
  CategorySet *catagorySet = m_CategorySetAllocator.allocate(1);
  new (catagorySet) CategorySet();
  pSymTab.setCategorySet(catagorySet);
}

void StrSymPool::addDirectClient(SymbolTableIF &pSymTab)
{
  pSymTab.setCategorySet(&m_CategorySet);
}

} // namespace of mcld
