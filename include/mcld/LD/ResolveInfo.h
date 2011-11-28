/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   ChinYen Chou <chinyen.chou@mediatek.com>                                *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_RESOLVE_INFO_H
#define MCLD_RESOLVE_INFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class ResolveInfo
 *  \brief ResolveInfo records the information about how to resolve a symbol.
 *
 *  A symbol must have some `attributes':
 *  - Type - Defined, Reference, Common or Indirect
 *  - Binding - Global, Local, Weak
 *  - IsDyn - appear in dynamic objects or regular objects
 *  In order to save the memory and speed up the performance, MCLinker uses
 *  a bit field to store all attributes.
 */
class ResolveInfo
{
public:
  enum Type {
    Defined,
    Reference,
    Common,
    Indirect,
    NoneType
  };

  enum Binding {
    Global,
    Local,
    Weak,
    NoneBinding
  };

  enum Visibility {
    Default = 0,
    Internal = 1,
    Hidden = 2,
    Protected = 3
  };

public:
  // -----  modifiers  ----- //
  void setDyn(bool pDyn = true);

  void setType(Type pType);

  void setBinding(Binding pBinding);

  void setOther(uint8_t pOther);

  void setVisibility(Visibility pVisibility);

  void overrideAttributes(const ResolveInfo& pFrom);

  void overrideVisibility(const ResolveInfo& pFrom);

  // -----  observers  ----- //
  bool isDyn() const;

  unsigned int type() const;

  unsigned int binding() const;

  uint8_t other() const
  { return (uint8_t)visibility(); }

  Visibility visibility() const;

  const char* name() const
  { return m_Name; }

private:
  const uint32_t DEF_BIT = 0x0;
  const uint32_t REF_BIT = 1 << 1;
  const uint32_t COM_BIT = 1 << 3;

  const uint32_t REG_BIT = 0x0;
  const uint32_t DYN_BIT = 1 << 1;

  const uint32_t GLOBAL_BIT = 0x0;
  const uint32_t WEAK_BIT = 1;
  const uint32_t LOCAL_BIT = 1 << 4;

  const uint32_t VISIBILITY = 0x3 << 5;

  const uint32_t RESOLVE_MASK = 0xF;

private:
  ResolveInfo();
  ResolveInfo(const ResolveInfo& pCopy);
  ResolveInfo& operator=(const ResolveInfo& pCopy);
  ~ResolveInfo();

private:
  /** m_BitField
   *  31     ...       8    7     6    ..      5   4    3   2   1   0
   * | length of m_Name |reserved|ELF visibility|Local|Com|Ref|Dyn|Weak|
   */
  uint32_t m_BitField;
  char m_Name[0];
};

} // namespace of mcld

#endif

