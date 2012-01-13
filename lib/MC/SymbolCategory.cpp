//===- SymbolCategory.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/SymbolCategory.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/ResolveInfo.h>

using namespace mcld;

//==========================
// SymbolCategory
SymbolCategory::SymbolCategory()
{
}

SymbolCategory::~SymbolCategory()
{
}

SymbolCategory& SymbolCategory::add(LDSymbol& pSymbol)
{
  if (pSymbol.desc() == ResolveInfo::Common) {
    m_CommonSymbols.push_back(&pSymbol);
    return *this;
  }

  if (pSymbol.binding() == ResolveInfo::Local) {
    m_LocalSymbols.push_back(&pSymbol);
    return *this;
  }

  m_RegSymbols.push_back(&pSymbol);
  return *this;
}

SymbolCategory& SymbolCategory::forceLocal(LDSymbol& pSymbol)
{
  m_ForceLocal.insert(&pSymbol);
  return *this;
}

size_t SymbolCategory::numOfSymbols() const
{
  return (m_ForceLocal.size() +
          m_LocalSymbols.size() +
          m_CommonSymbols.size() +
          m_RegSymbols.size());
}

size_t SymbolCategory::numOfForceLocals() const
{
  return m_ForceLocal.size();
}

size_t SymbolCategory::numOfLocals() const
{
  return m_LocalSymbols.size();
}

size_t SymbolCategory::numOfCommons() const
{
  return m_CommonSymbols.size();
}

size_t SymbolCategory::numOfRegulars() const
{
  return m_RegSymbols.size();
}

bool SymbolCategory::empty() const
{
  return (emptyForceLocals() &&
          emptyLocals() &&
          emptyCommons() &&
          emptyRegulars());
}

bool SymbolCategory::emptyForceLocals() const
{
  return m_ForceLocal.empty();
}

bool SymbolCategory::emptyLocals() const
{
  return m_LocalSymbols.empty();
}

bool SymbolCategory::emptyCommons() const
{
  return m_CommonSymbols.empty();
}

bool SymbolCategory::emptyRegulars() const
{
  return m_RegSymbols.empty();
}

SymbolCategory::iterator SymbolCategory::begin()
{
  return iterator(this, 0);
}

SymbolCategory::iterator SymbolCategory::end()
{
  return iterator(this, -1);
}

SymbolCategory::const_iterator SymbolCategory::begin() const
{
  return const_iterator(this, 0);
}

SymbolCategory::const_iterator SymbolCategory::end() const
{
  return const_iterator(this, -1);
}

SymbolCategory::force_local_iterator SymbolCategory::forceLocalBegin()
{
  return m_ForceLocal.begin();
}

SymbolCategory::force_local_iterator SymbolCategory::forceLocalEnd()
{
  return m_ForceLocal.end();
}

SymbolCategory::const_force_local_iterator SymbolCategory::forceLocalBegin() const
{
  return m_ForceLocal.begin();
}

SymbolCategory::const_force_local_iterator SymbolCategory::forceLocalEnd() const
{
  return m_ForceLocal.end();
}

SymbolCategory::class_iterator SymbolCategory::localBegin()
{
  return m_LocalSymbols.begin();
}

SymbolCategory::class_iterator SymbolCategory::localEnd()
{
  return m_LocalSymbols.end();
}

SymbolCategory::const_class_iterator SymbolCategory::localBegin() const
{
  return m_LocalSymbols.begin();
}

SymbolCategory::const_class_iterator SymbolCategory::localEnd() const
{
  return m_LocalSymbols.end();
}

SymbolCategory::class_iterator SymbolCategory::commonBegin()
{
  return m_CommonSymbols.begin();
}

SymbolCategory::class_iterator SymbolCategory::commonEnd()
{
  return m_CommonSymbols.end();
}

SymbolCategory::const_class_iterator SymbolCategory::commonBegin() const
{
  return m_CommonSymbols.begin();
}

SymbolCategory::const_class_iterator SymbolCategory::commonEnd() const
{
  return m_CommonSymbols.end();
}

SymbolCategory::class_iterator SymbolCategory::regularBegin()
{
  return m_RegSymbols.begin();
}

SymbolCategory::class_iterator SymbolCategory::regularEnd()
{
  return m_RegSymbols.end();
}

SymbolCategory::const_class_iterator SymbolCategory::regularBegin() const
{
  return m_RegSymbols.begin();
}

SymbolCategory::const_class_iterator SymbolCategory::regularEnd() const
{
  return m_RegSymbols.end();
}

