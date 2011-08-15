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
MCLDAttribute::MCLDAttribute(AttributeFactory& pParent, AttributeBase& pBase)
  : m_Parent(pParent), m_pBase(&pBase) {
}

MCLDAttribute::~MCLDAttribute()
{
}

bool MCLDAttribute::isWholeArchive() const
{
  if (m_Parent.constraint().isWholeArchive())
    return m_pBase->isWholeArchive();
  else
    return false;
}

bool MCLDAttribute::isAsNeeded() const
{
  if (m_Parent.constraint().isAsNeeded())
    return m_pBase->isAsNeeded();
  else
    return false;
}

bool MCLDAttribute::isStatic() const
{
  if (m_Parent.constraint().isStatic())
    return m_pBase->isStatic();
  else
    return true;
}

bool MCLDAttribute::isDynamic() const
{
  if (m_Parent.constraint().isStatic())
    return m_pBase->isDynamic();
  else
    return false;
}

static inline void ReplaceOrRecord(AttributeFactory& pParent,
                                   AttributeBase *&pBase,
                                   AttributeBase *&pCopy)
{
  AttributeBase *result = pParent.exists(*pCopy);
  if (0 == result) { // can not find
    pParent.record(*pCopy);
    pBase = pCopy;
  }
  else { // find
    delete pCopy;
    pBase = result;
  }
}

void MCLDAttribute::setWholeArchive()
{
  AttributeBase *copy = new AttributeBase(*m_pBase);
  copy->setWholeArchive();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void MCLDAttribute::unsetWholeArchive()
{
  AttributeBase *copy = new AttributeBase(*m_pBase);
  copy->unsetWholeArchive();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void MCLDAttribute::setAsNeeded()
{
  AttributeBase *copy = new AttributeBase(*m_pBase);
  copy->setAsNeeded();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void MCLDAttribute::unsetAsNeeded()
{
  AttributeBase *copy = new AttributeBase(*m_pBase);
  copy->unsetAsNeeded();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void MCLDAttribute::setStatic()
{
  AttributeBase *copy = new AttributeBase(*m_pBase);
  copy->setStatic();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

void MCLDAttribute::setDynamic()
{
  AttributeBase *copy = new AttributeBase(*m_pBase);
  copy->setDynamic();
  ReplaceOrRecord(m_Parent, m_pBase, copy);
}

MCLDAttribute* MCLDAttribute::clone() const
{
  return new MCLDAttribute(m_Parent, *m_pBase);
}

