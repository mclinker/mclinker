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
  : m_bWholeArchive(false), m_bAsNeeded(false) {
}

MCLDAttribute::~MCLDAttribute()
{
}

//==========================
// AttributeFactory
AttributeFactory::AttributeFactory(size_t pNum)
  : m_AttrSet() {
  m_AttrSet.reserve(pNum);
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

MCLDAttribute* AttributeFactory::produce()
{
  MCLDAttribute* result = new MCLDAttribute();
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

bool AttributeFactory::record(MCLDAttribute *&pAttr)
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

