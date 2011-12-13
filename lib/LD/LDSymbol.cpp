//===- LDSymbol.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/LDSymbol.h"
#include <cstring>

using namespace mcld;

LDSymbol::LDSymbol()
  : m_pResolveInfo(NULL), m_pFragRef(NULL) {
}

LDSymbol::~LDSymbol()
{
  if (NULL != m_pFragRef)
    delete m_pFragRef;
}

LDSymbol::LDSymbol(const LDSymbol& pCopy)
  : m_pResolveInfo(pCopy.m_pResolveInfo),
    m_pFragRef(pCopy.m_pFragRef) {
}

LDSymbol& LDSymbol::operator=(const LDSymbol& pCopy)
{
  m_pResolveInfo = pCopy.m_pResolveInfo;
  m_pFragRef = pCopy.m_pFragRef;
  return (*this);
}

void LDSymbol::setFragmentRef(llvm::MCFragment& pFragment, LDSymbol::Offset pOffset)
{
  if (NULL == m_pFragRef) {
    m_pFragRef = new MCFragmentRef(pFragment, pOffset);
    return;
  }
  m_pFragRef->assign(pFragment, pOffset);
}

void LDSymbol::setResolveInfo(const ResolveInfo& pInfo)
{
  m_pResolveInfo = const_cast<ResolveInfo*>(&pInfo);
}

