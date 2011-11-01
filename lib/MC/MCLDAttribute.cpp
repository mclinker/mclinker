//===- MCLDAttribute.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCLDAttribute.h"
#include "mcld/MC/AttributeFactory.h"

using namespace mcld;

//==========================
// AttrConstraint
bool AttrConstraint::isLegal(const Attribute& pAttr, std::string &pErrMesg) const
{
  if (!isWholeArchive() && pAttr.isWholeArchive()) {
    pErrMesg = std::string("Target does not support --whole-archive");
    return false;
  }
  if (!isAsNeeded() && pAttr.isAsNeeded()) {
    pErrMesg = std::string("Target does not support --as-needed");
    return false;
  }
  if (!isAddNeeded() && pAttr.isAddNeeded()) {
    pErrMesg = std::string("Target does not support --add-needed");
    return false;
  }
  if (isStaticSystem() && pAttr.isDynamic()) {
    pErrMesg = std::string("Target does not support --Bdynamic");
    return false;
  }
  // FIXME: may be it's legal, but ignored by GNU ld.
  if (isStaticSystem() && pAttr.isAsNeeded()) {
    pErrMesg = std::string("Can't enable --as-needed on a target which does not support dynamic linking");
    return false;
  }
  // FIXME: may be it's legal, but ignored by GNU ld.
  if (pAttr.isAsNeeded() && pAttr.isStatic()) {
    pErrMesg = std::string("Can't mix --static with --as-needed");
    return false;
  }
  return true;
}

//==========================
// AttributeProxy
AttributeProxy::AttributeProxy(AttributeFactory& pParent, Attribute& pBase)
  : m_AttrPool(pParent), m_pBase(&pBase) {
}

AttributeProxy::~AttributeProxy()
{
}

bool AttributeProxy::isWholeArchive() const
{
  if (m_AttrPool.constraint().isWholeArchive())
    return m_pBase->isWholeArchive();
  else
    return false;
}

bool AttributeProxy::isAsNeeded() const
{
  if (m_AttrPool.constraint().isAsNeeded())
    return m_pBase->isAsNeeded();
  else
    return false;
}

bool AttributeProxy::isAddNeeded() const
{
  if (m_AttrPool.constraint().isAddNeeded())
    return m_pBase->isAddNeeded();
  else
    return false;
}

bool AttributeProxy::isStatic() const
{
  if (m_AttrPool.constraint().isSharedSystem())
    return m_pBase->isStatic();
  else
    return true;
}

bool AttributeProxy::isDynamic() const
{
  if (m_AttrPool.constraint().isSharedSystem())
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
  ReplaceOrRecord(m_AttrPool, m_pBase, copy);
}

void AttributeProxy::unsetWholeArchive()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->unsetWholeArchive();
  ReplaceOrRecord(m_AttrPool, m_pBase, copy);
}

void AttributeProxy::setAsNeeded()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->setAsNeeded();
  ReplaceOrRecord(m_AttrPool, m_pBase, copy);
}

void AttributeProxy::unsetAsNeeded()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->unsetAsNeeded();
  ReplaceOrRecord(m_AttrPool, m_pBase, copy);
}

void AttributeProxy::setAddNeeded()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->setAddNeeded();
  ReplaceOrRecord(m_AttrPool, m_pBase, copy);
}

void AttributeProxy::unsetAddNeeded()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->unsetAddNeeded();
  ReplaceOrRecord(m_AttrPool, m_pBase, copy);
}

void AttributeProxy::setStatic()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->setStatic();
  ReplaceOrRecord(m_AttrPool, m_pBase, copy);
}

void AttributeProxy::setDynamic()
{
  Attribute *copy = new Attribute(*m_pBase);
  copy->setDynamic();
  ReplaceOrRecord(m_AttrPool, m_pBase, copy);
}

AttributeProxy* AttributeProxy::clone() const
{
  return new AttributeProxy(m_AttrPool, *m_pBase);
}

