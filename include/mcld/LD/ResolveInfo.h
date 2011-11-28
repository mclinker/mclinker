/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chinyen Chou <chinyen.chou@mediatek.com>                                *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_RESOLVE_INFO_H
#define MCLD_RESOLVE_INFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/DataTypes.h>
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
friend class ResolveInfoFactory;
public:
  typedef uint64_t ValueType;

  enum Type {
    Defined,
    Reference,
    Common,
    Indirect,
    NoneType
  };

  enum Binding {
    Global,
    Weak,
    Local,
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

  void setValue(ValueType pValue)
  { m_Value = pValue; }

  void overrideAttributes(const ResolveInfo& pFrom);

  void overrideVisibility(const ResolveInfo& pFrom);

  // -----  observers  ----- //
  bool isDyn() const;

  unsigned int type() const;

  unsigned int binding() const;

  uint8_t other() const
  { return (uint8_t)visibility(); }

  Visibility visibility() const;

  ValueType value() const
  { return m_Value; }

  const char* name() const
  { return m_Name; }

private:
  static const uint32_t TYPE_OFSET = 2;
  static const uint32_t TYPE_MASK = 0x3 << TYPE_OFSET;

  static const uint32_t DYN_OFFSET = 1;
  static const uint32_t DYN_MASK = 1 << DYN_OFFSET;

  static const uint32_t BINDING_OFFSET = 0;
  static const uint32_t BINDING_MASK = 1;
  static const uint32_t LOCAL_OFFSET = 4;
  static const uint32_t LOCAL_MASK = 1 << LOCAL_OFFSET;

  static const uint32_t VISIBILITY_OFFSET = 5;
  static const uint32_t VISIBILITY_MASK = 0x3 << VISIBILITY_OFFSET;

  static const uint32_t NAME_LENGTH_OFFSET = 8;
  static const uint32_t RESOLVE_MASK = 0xF;

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
  ValueType m_Value;
  char m_Name[0];
};

} // namespace of mcld

#endif

