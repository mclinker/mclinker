//===- SymbolCategory.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SYMBOL_CATEGORY_H
#define MCLD_SYMBOL_CATEGORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/ADT/TypeTraits.h>
#include <vector>

namespace mcld
{

class LDSymbol;
class ResolveInfo;
/** \class SymbolCategory
 *  \brief SymbolCategory groups output LDSymbol into different categories.
 */
class SymbolCategory
{
private:
  typedef std::vector<LDSymbol*> OutputSymbols;

public:
  typedef OutputSymbols::iterator iterator;
  typedef OutputSymbols::const_iterator const_iterator;

public:
  SymbolCategory();

  ~SymbolCategory();

  // -----  modifiers  ----- //
  SymbolCategory& add(LDSymbol& pSymbol);

  SymbolCategory& forceLocal(LDSymbol& pSymbol);

  SymbolCategory& arrange(LDSymbol& pSymbol, const ResolveInfo& pSourceInfo);

  SymbolCategory& changeCommonsToGlobal();

  // -----  access  ----- //
  LDSymbol& at(size_t pPosition)
  { return *m_OutputSymbols.at(pPosition); }

  const LDSymbol& at(size_t pPosition) const
  { return *m_OutputSymbols.at(pPosition); }

  LDSymbol& operator[](size_t pPosition)
  { return *m_OutputSymbols[pPosition]; }

  const LDSymbol& operator[](size_t pPosition) const
  { return *m_OutputSymbols[pPosition]; }

  // -----  observers  ----- //
  size_t numOfSymbols() const;

  size_t numOfLocals() const;

  size_t numOfCommons() const;

  size_t numOfRegulars() const;

  bool empty() const;

  bool emptyLocals() const;
  
  bool emptyCommons() const;

  bool emptyRegulars() const;

  // -----  iterators  ----- //
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  iterator localBegin();
  iterator localEnd();
  const_iterator localBegin() const;
  const_iterator localEnd() const;

  iterator commonBegin();
  iterator commonEnd();
  const_iterator commonBegin() const;
  const_iterator commonEnd() const;

  iterator regularBegin();
  iterator regularEnd();
  const_iterator regularBegin() const;
  const_iterator regularEnd() const;

private:
  class Category
  {
  public:
    enum Type {
      File,
      Local,
      Common,
      Weak,
      Global
    };

  public:
    Type type;

    size_t begin;
    size_t end;

    Category* prev;
    Category* next;

  public:
    Category(Type pType)
      : type(pType),
        begin(0),
        end(0),
        prev(NULL),
        next(NULL) {
    }

    size_t size() const
    { return (end - begin); }

    bool empty() const
    { return (begin == end); }

    bool isFirst() const
    { return (NULL == prev); }

    bool isLast() const
    { return (NULL == next); }

    static Type categorize(const ResolveInfo& pInfo);

  };

private:
  OutputSymbols m_OutputSymbols;

  Category* m_pFile;
  Category* m_pLocal;
  Category* m_pCommon;
  Category* m_pWeak;
  Category* m_pGlobal;
};

} // namespace of mcld

#endif

