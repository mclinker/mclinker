//===- LDSymbol.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDSymbol.h>

#include <mcld/Config/Config.h>
#include <mcld/Fragment/FragmentRef.h>
#include <mcld/Fragment/NullFragment.h>
#include <mcld/Support/GCFactory.h>

#include <cstring>

#include <llvm/Support/ManagedStatic.h>

using namespace mcld;

static LDSymbol* g_NullSymbol = NULL;

typedef GCFactory<LDSymbol, MCLD_SYMBOLS_PER_INPUT> LDSymbolFactory;

static llvm::ManagedStatic<LDSymbolFactory> g_LDSymbolFactory;

//===----------------------------------------------------------------------===//
// LDSymbol
//===----------------------------------------------------------------------===//
LDSymbol::LDSymbol()
  : m_pResolveInfo(NULL), m_pFragRef(NULL), m_Value(0) {
}

LDSymbol::~LDSymbol()
{
}

LDSymbol::LDSymbol(const LDSymbol& pCopy)
  : m_pResolveInfo(pCopy.m_pResolveInfo),
    m_pFragRef(pCopy.m_pFragRef),
    m_Value(pCopy.m_Value) {
}

LDSymbol& LDSymbol::operator=(const LDSymbol& pCopy)
{
  m_pResolveInfo = pCopy.m_pResolveInfo;
  m_pFragRef = pCopy.m_pFragRef;
  m_Value = pCopy.m_Value;
  return (*this);
}

LDSymbol* LDSymbol::Create(ResolveInfo& pResolveInfo)
{
  LDSymbol* result = g_LDSymbolFactory->allocate();
  new (result) LDSymbol();
  result->setResolveInfo(pResolveInfo);
  return result;
}

void LDSymbol::Destroy(LDSymbol*& pSymbol)
{
  pSymbol->~LDSymbol();
  g_LDSymbolFactory->deallocate(pSymbol);
  pSymbol = NULL;
}

void LDSymbol::Clear()
{
  g_LDSymbolFactory->clear();
}

LDSymbol* LDSymbol::Null()
{
  if (NULL == g_NullSymbol) {
    g_NullSymbol = new LDSymbol();
    g_NullSymbol->setResolveInfo(*ResolveInfo::Null());
    NullFragment* null_frag = new NullFragment();
    g_NullSymbol->setFragmentRef(FragmentRef::Create(*null_frag, 0));
    ResolveInfo::Null()->setSymPtr(g_NullSymbol);
  }
  return g_NullSymbol;
}

void LDSymbol::setFragmentRef(FragmentRef* pFragmentRef)
{
  m_pFragRef = pFragmentRef;
}

void LDSymbol::setResolveInfo(const ResolveInfo& pInfo)
{
  m_pResolveInfo = const_cast<ResolveInfo*>(&pInfo);
}

bool LDSymbol::isNull() const
{
  return (this == Null());
}

bool LDSymbol::hasFragRef() const
{
  return !m_pFragRef->isNull();
}

