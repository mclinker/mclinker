//===- LDSymbol.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/LDSymbol.h>

#include <cstring>

using namespace mcld;

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

void LDSymbol::setFragmentRef(FragmentRef* pFragmentRef)
{
  m_pFragRef = pFragmentRef;
}

void LDSymbol::setResolveInfo(const ResolveInfo& pInfo)
{
  m_pResolveInfo = const_cast<ResolveInfo*>(&pInfo);
}

