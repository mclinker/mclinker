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
  // Reference: Google gold linker: resolve.cc
  //
  // The rule for combining visibility is that we always choose the
  // most constrained visibility.  In order of increasing constraint,
  // visibility goes PROTECTED, HIDDEN, INTERNAL.  This is the reverse
  // of the numeric values, so the effect is that we always want the
  // smallest non-zero value.
  //
  // enum {
  //   STV_DEFAULT = 0,
  //   STV_INTERNAL = 1,
  //   STV_HIDDEN = 2,
  //   STV_PROTECTED = 3
  // };

  Visibility from_vis = pFrom.visibility();
  Visibility cur_vis = visibility();
  if (0 != from_vis ) {
    if (0 == cur_vis)
      setVisibility(from_vis);
    else if (cur_vis > from_vis)
      setVisibility(from_vis);
  }
}

void ResolveInfo::setRegular()
{
  m_BitField &= ~DYN_MASK;
}

void ResolveInfo::setDynamic()
{
  m_BitField |= dynamic_flag;
}

void ResolveInfo::setSource(bool pIsDyn)
{
  if (pIsDyn)
    m_BitField &= ~DYN_MASK;
  else
    m_BitField |= dynamic_flag;
}

void ResolveInfo::setType(uint32_t pType)
{
  m_BitField &= ~TYPE_MASK;
  m_BitField |= (pType << TYPE_OFFSET);
}

void ResolveInfo::setDesc(uint32_t pDesc)
{
  m_BitField &= ~DESC_MASK;
  m_BitField |= (pDesc << DESC_OFFSET);
}

void ResolveInfo::setBinding(uint32_t pBinding)
{
  m_BitField &= ~BINDING_MASK;
  if (pBinding == Local || pBinding == Absolute)
    m_BitField |= local_flag;
  if (pBinding == Weak || pBinding == Absolute)
    m_BitField |= weak_flag;
}

void ResolveInfo::setOther(uint32_t pOther)
{
  setVisibility(static_cast<ResolveInfo::Visibility>(pOther & 0x3));
}

void ResolveInfo::setVisibility(ResolveInfo::Visibility pVisibility)
{
  m_BitField &= ~VISIBILITY_MASK;
  m_BitField |= pVisibility << VISIBILITY_OFFSET;
}

void ResolveInfo::setHasPLT(bool pHasPLT)
{
  if (pHasPLT)
    m_BitField |= has_plt_flag;
  else
    m_BitField &= ~has_plt_flag;
}

void ResolveInfo::setIsSymbol(bool pIsSymbol)
{
  if (pIsSymbol)
    m_BitField |= is_symbol_flag;
  else
    m_BitField &= ~is_symbol_flag;
}

bool ResolveInfo::isDyn() const
{
  return (dynamic_flag == (m_BitField & DYN_MASK));
}

bool ResolveInfo::isUndef() const
{
  return (undefine_flag == (m_BitField & DESC_MASK));
}

bool ResolveInfo::isDefine() const
{
  return (define_flag == (m_BitField & DESC_MASK));
}

bool ResolveInfo::isCommon() const
{
  return (common_flag == (m_BitField & DESC_MASK));
}

bool ResolveInfo::isIndirect() const
{
  return (indirect_flag == (m_BitField & DESC_MASK));
}

// isGlobal - [L,W] == [0, 0]
bool ResolveInfo::isGlobal() const
{
  return (global_flag == (m_BitField & BINDING_MASK));
}

// isWeak - [L,W] == [0, 1]
bool ResolveInfo::isWeak() const
{
  return (weak_flag == (m_BitField & BINDING_MASK));
}

// isLocal - [L,W] == [1, 0]
bool ResolveInfo::isLocal() const
{
  return (local_flag == (m_BitField & BINDING_MASK));
}

// isAbsolute - [L,W] == [1, 1]
bool ResolveInfo::isAbsolute() const
{
  return (absolute_flag == (m_BitField & BINDING_MASK));
}

bool ResolveInfo::hasPLT() const
{
  return (has_plt_flag == (m_BitField & PLT_MASK));
}

bool ResolveInfo::isSymbol() const
{
  return (is_symbol_flag == (m_BitField & ISSYMBOL_MASK));
}

unsigned int ResolveInfo::type() const
{
  return (m_BitField & TYPE_MASK) >> TYPE_OFFSET;
}

unsigned int ResolveInfo::desc() const
{
  return (m_BitField & DESC_MASK) >> DESC_OFFSET;
}

unsigned int ResolveInfo::binding() const
{
  if (m_BitField & LOCAL_MASK) {
    if (m_BitField & GLOBAL_MASK) {
      return ResolveInfo::Absolute;
    }
    return ResolveInfo::Local;
  }
  return m_BitField & GLOBAL_MASK;
}

ResolveInfo::Visibility ResolveInfo::visibility() const
{
  return static_cast<ResolveInfo::Visibility>((m_BitField & VISIBILITY_MASK) >> VISIBILITY_OFFSET);
}

bool ResolveInfo::compare(const ResolveInfo::key_type& pKey)
{
  size_t length = nameSize();
  if (length != pKey.size())
    return false;
  return (0 == std::memcmp(m_Name, pKey.data(), length));
}

