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
#include <mcld/MC/CategoryIterator.h>
#include <vector>
#include <set>

namespace mcld
{

class LDSymbol;
/** \class SymbolCategory
 *  \brief SymbolCategory groups output LDSymbol into different categories.
 */
class SymbolCategory
{
private:
  typedef std::set<LDSymbol*> ForceLocalSet;
  typedef std::vector<LDSymbol*> OutputSymbols;

public:
  typedef CategoryIterator<NonConstTraits<LDSymbol>,
                           SymbolCategory,
                           NonConstIteratorTraits<ForceLocalSet>,
                           NonConstIteratorTraits<OutputSymbols> > iterator;

  typedef CategoryIterator<ConstTraits<LDSymbol>,
                           const SymbolCategory,
                           ConstIteratorTraits<ForceLocalSet>,
                           ConstIteratorTraits<OutputSymbols> > const_iterator;

  typedef ForceLocalSet::iterator force_local_iterator;
  typedef ForceLocalSet::const_iterator const_force_local_iterator;

  typedef OutputSymbols::iterator class_iterator;
  typedef OutputSymbols::const_iterator const_class_iterator;

public:
  SymbolCategory();

  ~SymbolCategory();

  // -----  modifiers  ----- //
  SymbolCategory& add(LDSymbol& pSymbol);

  SymbolCategory& forceLocal(LDSymbol& pSymbol);

  // -----  observers  ----- //
  size_t numOfSymbols() const;

  size_t numOfForceLocals() const;

  size_t numOfLocals() const;

  size_t numOfCommons() const;

  size_t numOfRegulars() const;

  bool empty() const;

  bool emptyForceLocals() const;

  bool emptyLocals() const;
  
  bool emptyCommons() const;

  bool emptyRegulars() const;

  // -----  iterators  ----- //
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  force_local_iterator forceLocalBegin();
  force_local_iterator forceLocalEnd();
  const_force_local_iterator forceLocalBegin() const;
  const_force_local_iterator forceLocalEnd() const;

  class_iterator localBegin();
  class_iterator localEnd();
  const_class_iterator localBegin() const;
  const_class_iterator localEnd() const;

  class_iterator commonBegin();
  class_iterator commonEnd();
  const_class_iterator commonBegin() const;
  const_class_iterator commonEnd() const;

  class_iterator regularBegin();
  class_iterator regularEnd();
  const_class_iterator regularBegin() const;
  const_class_iterator regularEnd() const;

private:
  ForceLocalSet m_ForceLocal;
  OutputSymbols m_LocalSymbols;
  OutputSymbols m_CommonSymbols;
  OutputSymbols m_RegSymbols;

};

} // namespace of mcld

#endif

