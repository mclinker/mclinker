/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDAttribute.h>
#include <mcld/MC/AttributeFactory.h>

using namespace mcld;

//==========================
// AttributeFactory
AttributeFactory::AttributeFactory()
  : m_AttrSet() {
  m_AttrSet.push_back(new mcld::Attribute());
  m_pLast = new AttributeProxy(*this, *m_AttrSet.front());
}

AttributeFactory::AttributeFactory(size_t pNum)
  : m_AttrSet() {
  m_AttrSet.reserve(pNum);
  m_AttrSet.push_back(new mcld::Attribute());
  m_pLast = new AttributeProxy(*this, *m_AttrSet.front());
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

Attribute &AttributeFactory::predefined()
{
  return *m_AttrSet.front();
}

const Attribute &AttributeFactory::predefined() const
{
  return *m_AttrSet.front();
}

AttributeProxy* AttributeFactory::produce()
{
  m_pLast->change(m_AttrSet.front());
  return m_pLast->clone();
}

AttributeProxy& AttributeFactory::last()
{
  return *m_pLast;
}

const AttributeProxy& AttributeFactory::last() const
{
  return *m_pLast;
}

Attribute* AttributeFactory::exists(const Attribute& pAttr) const
{
  const_iterator cur = m_AttrSet.begin();
  const_iterator aEnd = m_AttrSet.end();
  while(cur != aEnd) {
    if (*(*cur) == pAttr) {
      m_pLast->change(*cur);
      return *cur;
    }
    ++cur;
  }
  return 0;
}

void AttributeFactory::record(mcld::Attribute &pAttr)
{
  m_AttrSet.push_back(&pAttr);
  m_pLast->change(m_AttrSet.back());
}

