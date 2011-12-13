//===- ResolveInfo.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ResolveInfo.h"
#include <cstring>

using namespace mcld;

//==========================
// ResolveInfo
ResolveInfo::ResolveInfo()
  : m_BitField(0), m_Size(0) {
  m_Value.value = 0;
}

ResolveInfo::~ResolveInfo()
{
}

void ResolveInfo::override(const ResolveInfo& pFrom)
{
  m_Size = pFrom.m_Size;
  m_Value.value = pFrom.m_Value.value;
  overrideAttributes(pFrom);
  overrideVisibility(pFrom);
}

void ResolveInfo::overrideAttributes(const ResolveInfo& pFrom)
{
  m_BitField &= ~RESOLVE_MASK;
  m_BitField |= (pFrom.m_BitField & RESOLVE_MASK);
}

/// overrideVisibility - override the visibility
///   always use the most strict visibility
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
  m_BitField |= pType << TYPE_OFFSET;
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

bool ResolveInfo::hasAttributes() const
{
  return (0 != (RESOLVE_MASK & m_BitField));
}

bool ResolveInfo::isDyn() const
{
  return (0 != (m_BitField & DYN_MASK));
}

bool ResolveInfo::isDefine() const
{
  return (0 != (m_BitField & define_flag));
}

bool ResolveInfo::isUndef() const
{
  return !isDefine();
}

bool ResolveInfo::isWeak() const
{
  return (0 != (m_BitField & weak_flag));
}

bool ResolveInfo::isCommon() const
{
  return (0 != (m_BitField & common_flag));
}

bool ResolveInfo::isIndirect() const
{
  return (0 != (m_BitField & indirect_flag));
}

unsigned int ResolveInfo::type() const
{
  return (m_BitField & TYPE_MASK) >> TYPE_OFFSET;
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
  return static_cast<ResolveInfo::Visibility>(val);
}

bool ResolveInfo::compare(const ResolveInfo::key_type& pKey)
{
  size_t length = nameSize();
  if (length != pKey.size())
    return false;
  return (0 == std::memcmp(m_Name, pKey.data(), length));
}

