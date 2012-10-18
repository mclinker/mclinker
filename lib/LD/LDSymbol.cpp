//===- LDSymbol.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/LDSymbol.h>
#include <mcld/Fragment/FragmentRef.h>
#include <mcld/Fragment/NullFragment.h>

#include <cstring>

using namespace mcld;

static LDSymbol* g_NullSymbol = NULL;

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

LDSymbol* LDSymbol::Null()
{
  if (NULL == g_NullSymbol) {
    g_NullSymbol = new LDSymbol();
    g_NullSymbol->setResolveInfo(*ResolveInfo::Null());
    NullFragment* null_frag = new NullFragment();
    g_NullSymbol->setFragmentRef(FragmentRef::Create(*null_frag, 0));
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

