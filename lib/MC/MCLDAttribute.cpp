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
// MCAttribute
AttributeProxy::AttributeProxy(AttributeFactory& pParent, Attribute& pBase)
  : m_Parent(pParent), m_pBase(&pBase) {
}

AttributeProxy::~AttributeProxy()
{
}

bool AttributeProxy::isWholeArchive() const
{
  if (m_Parent.constraint().isWholeArchive())
    return m_pBase->isWholeArchive();
  else
    return false;
}

bool AttributeProxy::isAsNeeded() const
{
  if (m_Parent.constraint().isAsNeeded())
    return m_pBase->isAsNeeded();
  else
    return false;
}

bool AttributeProxy::isStatic() const
{
  if (m_Parent.constraint().isStatic())
    return m_pBase->isStatic();
  else
    return true;
}

bool AttributeProxy::isDynamic() const
{
  if (m_Parent.constraint().isStatic())
    return m_pBase->isDynamic();
  else
    return false;
}

static inline void ReplaceOrRecord(AttributeFactory& pParent,
                                   Attribute *&pBase,
                                   Attribute *&pCopy)
{
  Attribute *result = pParent.exists(*pCopy);
  if (0 == result) { // can not find
    pParent.record(*pCopy);
    pBase = pCopy;
  }
  else { // find
    delete pCopy;
    pBase = result;
  }
}

void AttributeProxy::setWholeArchive()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->setWholeArchive();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void AttributeProxy::unsetWholeArchive()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->unsetWholeArchive();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void AttributeProxy::setAsNeeded()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->setAsNeeded();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void AttributeProxy::unsetAsNeeded()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->unsetAsNeeded();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void AttributeProxy::setStatic()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->setStatic();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void AttributeProxy::setDynamic()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->setDynamic();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

AttributeProxy* AttributeProxy::clone() const
{
  return new AttributeProxy(m_Parent, *m_pBase);
}

