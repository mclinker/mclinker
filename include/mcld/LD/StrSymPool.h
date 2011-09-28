/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef STRSYMPOOL_H
#define STRSYMPOOL_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/Uncopyable.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/LD/LDSymbol.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/Allocator.h>
#include <vector>

namespace llvm
{
  class MCSectionData;
}

namespace mcld
{

class StringTableIF;
class SymbolTableIF;
class Resolver;
class Resolver {
  /* FIXME: remove this. */
public:
  bool resolve(const LDSymbol &sym1, const LDSymbol &sym2) {
    return false;
  }
};

/** \class StrSymPool
 *  \brief Store symbol and search symbol by name. Can help symbol resolution.
 *
 *  - SectLinker is responsed for creating StrSymPool.
 */
class StrSymPool : private Uncopyable
{
private:
  class EntryType : private Uncopyable
  {
    friend class StrSymPool;
  private:
    EntryType(){}

  public:
    void addSameNameSymbol(LDSymbol &new_sym, Resolver &pResolver) {
      if(!pResolver.resolve(m_Symbol, new_sym)) {
        m_Sections.push_back(new_sym.section());
      }
      else {
        m_Sections.push_back(m_Symbol.section());

        m_Symbol.setDynamic(new_sym.isDyn());
        m_Symbol.setType(new_sym.type());
        m_Symbol.setBinding(new_sym.binding());
        m_Symbol.setSection(new_sym.section());
      }
    }

    LDSymbol &symbol()
    { return m_Symbol; }

    const LDSymbol &symbol() const
    { return m_Symbol; }

  private:
    LDSymbol m_Symbol;
    std::vector<const llvm::MCSectionData *> m_Sections;
  }; // end of EntryType

  typedef llvm::StringMap<EntryType *, llvm::BumpPtrAllocator> SearcherType;

public:
  typedef std::vector<LDSymbol *> SymbolCatagory;
  class CatagorySet
  {
  public:
    enum Type {
      Entire,
      Dynamic,
      Common,
      NumOfCatagories
    };
    typedef StrSymPool::SymbolCatagory       SymbolCatagory;

  public:
    SymbolCatagory& at(size_t pType) {
      assert(pType<NumOfCatagories);
      return m_SymbolCatagorySet[pType];
    }

    const SymbolCatagory& at(size_t pType) const {
      assert(pType<NumOfCatagories);
      return m_SymbolCatagorySet[pType];
    }

    SymbolCatagory& operator[](size_t pType) {
      return m_SymbolCatagorySet[pType];
    }

    const SymbolCatagory& operator[](size_t pType) const {
      return m_SymbolCatagorySet[pType];
    }

    void interpose(StringTableIF *pStrTab) {
      /* TODO FIXME */
    }

    void insertSymbolPointer(LDSymbol *pSym) {
      m_SymbolCatagorySet[Entire].push_back(pSym);
      if (pSym->isDyn())
        m_SymbolCatagorySet[Dynamic].push_back(pSym);
      if (pSym->type() == LDSymbol::Common)
        m_SymbolCatagorySet[Common].push_back(pSym);
    }

  private:
    SymbolCatagory m_SymbolCatagorySet[NumOfCatagories];
  };

public:
  /// StrSymPool - constructor
  StrSymPool(size_t pNumOfSymbols,
                 size_t pNumOfInputs,
                 Resolver &pResolver);

  /// ~StrSymPool - destructor, all allcator destroy its own memory.
  ~StrSymPool()
  { }

  LDSymbol *insertSymbol(const char *pName,
                         bool pIsDynamic,
                         LDSymbol::Type pSymbolType,
                         LDSymbol::Binding pSymbolBinding,
                         const llvm::MCSectionData * pSection);

  const char *insertString(const char *pStr);

  void merge(const StrSymPool &pAnotherStrSymPool);
  void addIndirectClient(SymbolTableIF &pSymbolTable);
  void addDirectClient(SymbolTableIF &pSymbolTable);

private:
  GCFactory<EntryType, 0> m_EntryAllocator;
  GCFactory<LDSymbol, 0> m_SymbolAllocator;
  GCFactory<CatagorySet, 0> m_CatagorySetAllocator;
  SearcherType m_SymbolSearch;
  Resolver m_Resolver;

  CatagorySet m_CatagorySet;
};

} // namespace of mcld

#endif

