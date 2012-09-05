//===- AttributeFactory.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/MCLDAttribute.h>
#include <mcld/MC/AttributeFactory.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// AttributeFactory
//===----------------------------------------------------------------------===//
AttributeFactory::AttributeFactory()
  : m_AttrSet() {
  m_pPredefined = new Attribute();
  m_pLast = new AttributeProxy(*this, *m_pPredefined);

  m_AttrSet.push_back(m_pPredefined);
}

AttributeFactory::AttributeFactory(size_t pNum)
  : m_AttrSet() {
  m_AttrSet.reserve(pNum);

  m_pPredefined = new Attribute();
  m_pLast = new AttributeProxy(*this, *m_pPredefined);

  m_AttrSet.push_back(m_pPredefined);
}

AttributeFactory::~AttributeFactory()
{
  iterator cur = m_AttrSet.begin();
  iterator aEnd = m_AttrSet.end();
  while(cur != aEnd) {
    delete (*cur);
    ++cur;
  }
  m_AttrSet.clear();
  delete m_pLast;
}

void AttributeFactory::reserve(size_t pNum)
{
  m_AttrSet.reserve(pNum);
}

AttributeProxy* AttributeFactory::produce()
{
  m_pLast->assign(m_pPredefined);
  return m_pLast->clone();
}

Attribute* AttributeFactory::exists(const Attribute& pAttr) const
{
  const_iterator cur = m_AttrSet.begin();
  const_iterator aEnd = m_AttrSet.end();
  while(cur != aEnd) {
    if (*(*cur) == pAttr) {
      return *cur;
    }
    ++cur;
  }
  return 0;
}

void AttributeFactory::record(mcld::Attribute &pAttr)
{
  m_AttrSet.push_back(&pAttr);
}

