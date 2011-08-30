/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LD_SYMBOL_TABLE_H
#define LD_SYMBOL_TABLE_H
#include <cassert>
#include <vector>
#include <algorithm>
#include <mcld/ADT/StringMap.h>
#include <mcld/ADT/Uncopyable.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/StringTable.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

namespace symtab
{

// Map llvm::StringRef to EntryTy
template <class EntryTy>
class StringMapSearch : private Uncopyable
{
public:
  typedef typename StringMap<EntryTy>::iterator iterator;
public:
  StringMapSearch() {}
  ~StringMapSearch() {}  

public:
  // Interface for SymbolTable used
  EntryTy& get(llvm::StringRef pStr) const
  { return *m_Map.GetOrCreateValue(pStr); }

  // Interface for SymbolTable used
  EntryTy& create(llvm::StringRef pStr)
  { return *m_Map.GetOrCreateValue(pStr); }

  // Interface for SymbolTable used
  iterator begin()
  { return m_Map.begin(); }

  // Interface for SymbolTable used
  iterator end()
  { return m_Map.end(); }

private:
  StringMap<EntryTy> m_Map;
};


// default symbol table entry
class SymbolVector
{
public:
  SymbolVector()
  : m_pVec(new std::vector<LDSymbol>()) {}

  SymbolVector(const SymbolVector& pRHS)
  : m_pVec(pRHS.m_pVec) {}

  SymbolVector& operator=(const SymbolVector& pRHS)
  { m_pVec = pRHS.m_pVec; }

  ~SymbolVector()
  { delete m_pVec;  }

public:
  // Interface for SymbolTable used
  size_t size() const
  { return m_pVec->size();  }

  // Interface for SymbolTable used
  LDSymbol& getTopSymbol()
  { return (*m_pVec)[0];  }

  // Interface for SymbolTable used
  LDSymbol& getLastSymbol()
  { return (*m_pVec)[size()-1]; }

  // Interface for SymbolTable used
  void sort()
  { std::sort(m_pVec->begin(), m_pVec->end());  }

private:
  std::vector<LDSymbol>* m_pVec;
};

} // namespace symtab


/** \class SymbolTable
 *  \brief
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
template <template <class> class SymSearchStrategy = symtab::StringMapSearch,
          class SymTabEntryTy = symtab::SymbolVector>
class SymbolTable : private Uncopyable
{
public:
  typedef typename SymSearchStrategy<SymTabEntryTy>::iterator iterator;

public:
  SymbolTable(StringTable& pTab)
  : m_pStrTable(pTab) {}

  ~SymbolTable() {}

public:
  // for symbol resolution on input module
  const LDSymbol& getSymbol(llvm::StringRef pStr) const
  {
    SymTabEntryTy& Entry = m_SearchStrategy.get(pStr);
    assert(Entry.size() > 0 && "List of LDSymbol has not been created.");
    LDSymbol& Sym = Entry.getTopSymbol();
    assert(Sym.m_pStr && "Invalid LDSymbol.");
    return Sym;
  }

  // for input module reader
  LDSymbol& insertUnfilledSymbol(llvm::StringRef pStr)
  {
    SymTabEntryTy& Entry = m_SearchStrategy.create(pStr);
    assert(Entry.size() > 0 && "List of LDSymbol has not been created.");
    LDSymbol& Sym = Entry.getLastSymbol();
    assert(!Sym.m_pStr && "We need un-initialized LDSymbol.");
    return Sym;
  }

  // for symbol resolution on output module
  // move the symbol we want into output symbol table
  void insertSymbol(LDSymbol& pSym)
  {
    assert(pSym.m_pStr && "Parameter should have been initializaed.");
    llvm::StringRef Str(pSym.m_pStr);
    SymTabEntryTy& Entry = m_SearchStrategy.create(Str);
    assert(Entry->size() > 0 && "List of LDSymbol has not been created.");
    Entry.getLastSymbol() = pSym;
    sort(Entry);
  }

  iterator begin()
  { return m_SearchStrategy.begin();  }

  iterator end()
  { return m_SearchStrategy.end(); }

private:
  void sort(SymTabEntryTy& pEntry)
  { pEntry.sort();  }

private:
  SymSearchStrategy<SymTabEntryTy> m_SearchStrategy;
  StringTable& m_pStrTable;
};

} // namespace of mcld

#endif
