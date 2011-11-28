/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chinyen Chou <chinyen.chou@mediatek.com>                                *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/LD/ResolveInfo.h>

using namespace mcld;

//==========================
// ResolveInfo
ResolveInfo::ResolveInfo()
  : m_BitField(0), m_Value(0) {
}

ResolveInfo::~ResolveInfo()
{
  m_BitField = 0x0;
}

void ResolveInfo::overrideAttributes(const ResolveInfo& pFrom)
{
  m_BitField &= ~RESOLVE_MASK;
  m_BitField |= (pFrom.m_BitField & RESOLVE_MASK);
}

void ResolveInfo::overrideVisibility(const ResolveInfo& pFrom)
{
  ResolveInfo::Visibility vis =
    static_cast<ResolveInfo::Visibility>(pFrom.m_BitField & VISIBILITY_MASK);
  setVisibility(vis);
}

void ResolveInfo::setDyn(bool pDyn)
{
  if (pDyn)
    m_BitField |= DYN_MASK << DYN_OFFSET;
  else
    m_BitField &= ~DYN_MASK;
}

void ResolveInfo::setType(ResolveInfo::Type pType)
{
  m_BitField &= ~TYPE_MASK;
  m_BitField |= pType << TYPE_OFSET;
}

void ResolveInfo::setBinding(ResolveInfo::Binding pBinding)
{
  m_BitField &= ~(BINDING_MASK | LOCAL_MASK);
  if (pBinding == ResolveInfo::Local)
    m_BitField |= LOCAL_MASK;
  else if (pBinding == ResolveInfo::Weak)
    m_BitField |= BINDING_MASK;
}

void ResolveInfo::setOther(uint8_t pOther)
{
  setVisibility(static_cast<ResolveInfo::Visibility>(pOther & 0x3));
}

void ResolveInfo::setVisibility(ResolveInfo::Visibility pVisibility)
{
  m_BitField &= ~VISIBILITY_MASK;
  m_BitField |= pVisibility << VISIBILITY_OFFSET;
}

bool ResolveInfo::isDyn() const
{
  return (m_BitField & DYN_MASK) >> DYN_OFFSET;
}

unsigned int ResolveInfo::type() const
{
  return (m_BitField & TYPE_MASK) >> TYPE_OFSET;
}

unsigned int ResolveInfo::binding() const
{
  if (m_BitField & LOCAL_MASK) {
    if (m_BitField & BINDING_MASK) {
      return ResolveInfo::NoneBinding;
    }
    return ResolveInfo::Local;
  }
  return m_BitField & BINDING_MASK;
}

ResolveInfo::Visibility ResolveInfo::visibility() const
{
  uint8_t val = (m_BitField & ~VISIBILITY_MASK) >> VISIBILITY_OFFSET;
  ResolveInfo::Visibility vis = static_cast<ResolveInfo::Visibility>(val);
  return vis;
}

