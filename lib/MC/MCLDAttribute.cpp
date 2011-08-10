/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDAttribute.h>

using namespace mcld;

//==========================
// MCAttribute
MCLDAttribute::MCLDAttribute()
  : m_bWholeArchive(false), m_bAsNeeded(false), m_bStatic(true) {
}

MCLDAttribute::MCLDAttribute(const MCLDAttribute& pAttr)
  : m_bWholeArchive(pAttr.m_bWholeArchive),
    m_bAsNeeded(pAttr.m_bAsNeeded),
    m_bStatic(pAttr.m_bStatic) {
}

MCLDAttribute::~MCLDAttribute()
{
}

//==========================
// AttributeFactory
AttributeFactory::AttributeFactory(size_t pNum)
  : m_AttrSet(), m_pDefaultAttribute(0) {
  m_AttrSet.reserve(pNum);
}

AttributeFactory::AttributeFactory()
  : m_AttrSet(), m_pDefaultAttribute(0) {
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
}

void AttributeFactory::reserve(size_t pNum)
{
  m_AttrSet.reserve(pNum);
}

void AttributeFactory::setDefault(const MCLDAttribute& pAttr)
{
  m_pDefaultAttribute = const_cast<MCLDAttribute*>(&pAttr);
  recordOrReplace(m_pDefaultAttribute);
}

MCLDAttribute* AttributeFactory::produce()
{
  MCLDAttribute *result = 0;
  if (0 == m_pDefaultAttribute )
    result = new MCLDAttribute();
  else
    result = new MCLDAttribute(*m_pDefaultAttribute);
  return result;
}

MCLDAttribute* AttributeFactory::find(const MCLDAttribute& pAttr) const
{
  const_iterator cur = m_AttrSet.begin();
  const_iterator aEnd = m_AttrSet.end();
  while(cur != aEnd) {
    if (*(*cur) == pAttr)
      return *cur;
    ++cur;
  }
  return 0;
}

bool AttributeFactory::recordOrReplace(MCLDAttribute *&pAttr)
{
  if (0 == pAttr)
    return false;
  MCLDAttribute* result = find(*pAttr);
  if (0 == result) {
    m_AttrSet.push_back(pAttr);
    return true;
  }
  delete pAttr;
  pAttr = result;
  return false;
}

