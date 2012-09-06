//===- AttributeSet.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/Attribute.h>
#include <mcld/MC/AttributeSet.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// AttributeSet
//===----------------------------------------------------------------------===//
AttributeSet::AttributeSet(size_t pNum, Attribute& pPredefined)
  : m_AttrSet(), m_Predefined(pPredefined) {
  m_AttrSet.reserve(pNum);
}

AttributeSet::~AttributeSet()
{
  iterator cur = m_AttrSet.begin();
  iterator aEnd = m_AttrSet.end();

  while(cur != aEnd) {
    delete (*cur);
    ++cur;
  }
}

Attribute* AttributeSet::exists(const Attribute& pAttr) const
{
  if (m_Predefined == pAttr)
    return &m_Predefined;

  const_iterator cur = m_AttrSet.begin();
  const_iterator aEnd = m_AttrSet.end();
  while(cur != aEnd) {
    if (*(*cur) == pAttr) {
      return *cur;
    }
    ++cur;
  }
  return NULL;
}

void AttributeSet::record(mcld::Attribute &pAttr)
{
  m_AttrSet.push_back(&pAttr);
}

