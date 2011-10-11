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
#include <mcld/LD/Resolver.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/Allocator.h>
#include <vector>

namespace llvm
{
  class MCSectionData;
}

namespace mcld
{

class StringTable;
class SymbolTableIF;


/** \class SymbolCategorySet
 *  \brief 
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class SymbolCategorySet
{
public:
  enum Type {
    Entire,
    Dynamic,
    Common,
    NumOfCategories
  };
  class SymbolCategory : private Uncopyable
  {
  public:
    typedef LDSymbol *                   value_type;

  private:
    friend class SymbolCategorySet;
    typedef std::vector<value_type>      SymbolList;

  public:
    typedef SymbolList::size_type        size_type;
    typedef SymbolList::iterator         iterator;
    typedef SymbolList::const_iterator   const_iterator;
    typedef SymbolList::reference        reference;
    typedef SymbolList::const_reference  const_reference;

  private:
    SymbolCategory()
      : m_Strtab(0)
    {}

    ~SymbolCategory()
    {}

    void push_back(const value_type &pVal);

  public:

    iterator begin()
    { return m_Symbols.begin(); }

    const_iterator begin() const
    { return m_Symbols.begin(); }

    iterator end()
    { return m_Symbols.end(); }

    const_iterator end() const
    { return m_Symbols.end(); }

    reference operator[](size_type pIndex)
    { return m_Symbols[pIndex]; }

    const_reference operator[](size_type pIndex) const
    { return m_Symbols[pIndex]; }

    iterator erase(iterator pPosition)
    { return m_Symbols.erase(pPosition); }

    void reserve(size_type pSize)
    { m_Symbols.reserve(pSize); }

    size_type size() const
    { return m_Symbols.size(); }

    template<typename InputIterator>
    void insert(iterator pPosition, InputIterator pBegin, InputIterator pEnd)
    { m_Symbols.insert(pPosition, pBegin, pEnd); }

  public:
    void interpose(StringTable *pStrTab)
    { m_Strtab = pStrTab; }

  private:
    StringTable *m_Strtab;
    SymbolList m_Symbols;
  };

public:
  SymbolCategory& at(size_t pType) {
    assert(pType<NumOfCategories);
    return m_SymbolCategorySet[pType];
  }

  const SymbolCategory& at(size_t pType) const {
    assert(pType<NumOfCategories);
    return m_SymbolCategorySet[pType];
  }

  SymbolCategory& operator[](size_t pType) {
    return m_SymbolCategorySet[pType];
  }

  const SymbolCategory& operator[](size_t pType) const {
    return m_SymbolCategorySet[pType];
  }

  void insertSymbolPointer(LDSymbol *pSym) {
    m_SymbolCategorySet[Entire].push_back(pSym);
    if (pSym->isDyn())
      m_SymbolCategorySet[Dynamic].push_back(pSym);
    if (pSym->type() == LDSymbol::Common)
      m_SymbolCategorySet[Common].push_back(pSym);
  }

  void moveSymbolToNewCategory(const LDSymbol &pOldSym, const LDSymbol &pNewSym) {
    if (pOldSym.type() == LDSymbol::Common &&
        pNewSym.type() != LDSymbol::Common) {
      /* The only one situation we should move catagory is from Common to
       * other, so we just remove the old symbol from Common.
       */
      m_SymbolCategorySet[Common].erase(
        find(m_SymbolCategorySet[Common].begin(),
             m_SymbolCategorySet[Common].end(),
             &pOldSym));
    }
  }

private:
  SymbolCategory m_SymbolCategorySet[NumOfCategories];
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
    EntryType(const LDSymbol& pSym)
      : m_Symbol(pSym)
    {}

  public:
    void overrideSymbol(LDSymbol &new_sym) {
      m_Sections.push_back(m_Symbol.section());

      m_Symbol = new_sym;
    }

    void addReferenceSection(const llvm::MCSectionData *pSection) {
      m_Sections.push_back(pSection);
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
  typedef SymbolCategorySet                 CategorySet;
  typedef CategorySet::SymbolCategory       SymbolCategory;

public:
  /// StrSymPool - constructor
  StrSymPool(size_t pNumOfSymbols,
             size_t pNumOfInputs,
             Resolver pResolver);

  /// ~StrSymPool - destructor, all allcator destroy its own memory.
  ~StrSymPool()
  { }

  LDSymbol *insertSymbol(const char *pName,
                         bool pIsDynamic,
                         LDSymbol::Type pSymbolType,
                         LDSymbol::Binding pSymbolBinding,
                         const llvm::MCSectionData * pSection,
                         uint64_t pValue,
                         uint64_t pSize,
                         uint8_t pOther);

  const char *insertString(const char *pStr);

  void merge(const StrSymPool &pAnotherStrSymPool);
  void addIndirectClient(SymbolTableIF &pSymbolTable);
  void addDirectClient(SymbolTableIF &pSymbolTable);

private:
  GCFactory<EntryType, 0> m_EntryAllocator;
  GCFactory<LDSymbol, 0> m_SymbolAllocator;
  GCFactory<CategorySet, 0> m_CategorySetAllocator;
  SearcherType m_SymbolSearch;
  Resolver m_Resolver;

  CategorySet m_CategorySet;
};

} // namespace of mcld

#endif

