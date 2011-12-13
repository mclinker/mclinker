//===- ResolveInfo.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_RESOLVE_INFO_H
#define MCLD_RESOLVE_INFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "llvm/Support/DataTypes.h"
#include "llvm/ADT/StringRef.h"

namespace mcld
{

/** \class ResolveInfo
 *  \brief ResolveInfo records the information about how to resolve a symbol.
 *
 *  A symbol must have some `attributes':
 *  - Type - Defined, Reference, Common or Indirect
 *  - Binding - Global, Local, Weak
 *  - IsDyn - appear in dynamic objects or regular objects
 *  - Value - the value of the symbol
 *  In order to save the memory and speed up the performance, MCLinker uses
 *  a bit field to store all attributes.
 */
class ResolveInfo
{
friend class ResolveInfoFactory;
public:
  typedef uint64_t SizeType;
  typedef uint64_t ValueType;

  enum Type {
    Undefined,
    Define,
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

  // -----  For HashTable  ----- //
  typedef llvm::StringRef key_type;

public:
  // -----  modifiers  ----- //
  void setDyn(bool pDyn = true);

  void setType(Type pType);

  void setBinding(Binding pBinding);

  void setOther(uint8_t pOther);

  void setVisibility(Visibility pVisibility);

  void setValue(ValueType pValue)
  { m_Value.value = pValue; }

  void setLink(ResolveInfo* pTarget) {
    m_Value.ptr = pTarget;
    m_BitField |= indirect_flag;
  }

  void setSize(SizeType pSize)
  { m_Size = pSize; }

  void override(const ResolveInfo& pForm);

  void overrideAttributes(const ResolveInfo& pFrom);

  void overrideVisibility(const ResolveInfo& pFrom);

  // -----  observers  ----- //
  bool hasAttributes() const;

  bool isWeak() const;

  bool isDefine() const;

  bool isUndef() const;

  bool isDyn() const;

  bool isCommon() const;

  bool isIndirect() const;

  unsigned int type() const;

  unsigned int binding() const;

  uint8_t other() const
  { return (uint8_t)visibility(); }

  Visibility visibility() const;

  ValueType value() const
  { return m_Value.value; }

  ResolveInfo* link()
  { return m_Value.ptr; }

  const ResolveInfo* link() const
  { return m_Value.ptr; }

  SizeType size() const
  { return m_Size; }

  const char* name() const
  { return m_Name; }

  unsigned int nameSize() const
  { return (m_BitField >> NAME_LENGTH_OFFSET); }

  uint32_t info() const
  { return (m_BitField & RESOLVE_MASK); }

  // -----  For HashTable  ----- //
  bool compare(const key_type& pKey);

private:
  static const uint32_t BINDING_OFFSET = 0;
  static const uint32_t BINDING_MASK = 1;

  static const uint32_t DYN_OFFSET = 1;
  static const uint32_t DYN_MASK = 1 << DYN_OFFSET;

  static const uint32_t TYPE_OFFSET = 2;
  static const uint32_t TYPE_MASK = 0x3 << TYPE_OFFSET;

  static const uint32_t LOCAL_OFFSET = 4;
  static const uint32_t LOCAL_MASK = 1 << LOCAL_OFFSET;

  static const uint32_t VISIBILITY_OFFSET = 5;
  static const uint32_t VISIBILITY_MASK = 0x3 << VISIBILITY_OFFSET;

  static const uint32_t TOUCH_OFFSET = 7;
  static const uint32_t TOUCH_MASK = 1 << TOUCH_OFFSET;

  static const uint32_t NAME_LENGTH_OFFSET = 8;
  static const uint32_t RESOLVE_MASK = 0xF;

  union ValOrPtr {
    ValueType value;
    ResolveInfo* ptr;
  };

public:
  static const uint32_t global_flag = 0 << BINDING_OFFSET;
  static const uint32_t weak_flag = 1 << BINDING_OFFSET;
  static const uint32_t regular_flag = 0 << DYN_OFFSET;
  static const uint32_t dynamic_flag = 1 << DYN_OFFSET;
  static const uint32_t undefine_flag = 0 << TYPE_OFFSET;
  static const uint32_t define_flag = 1 << TYPE_OFFSET;
  static const uint32_t common_flag = 2 << TYPE_OFFSET;
  static const uint32_t indirect_flag = 3 << TYPE_OFFSET;
  static const uint32_t local_flag = 1 << LOCAL_OFFSET;
  static const uint32_t touch_flag = 1 << TOUCH_OFFSET;

private:
  ResolveInfo();
  ResolveInfo(const ResolveInfo& pCopy);
  ResolveInfo& operator=(const ResolveInfo& pCopy);
  ~ResolveInfo();

private:
  SizeType m_Size;
  ValOrPtr m_Value;
  /** m_BitField
   *  31     ...       8    7     6    ..      5   4    3   2   1   0
   * | length of m_Name |touched|ELF visibility|Local|Com|Def|Dyn|Weak|
   */
  uint32_t m_BitField;
  char m_Name[0];
};

} // namespace of mcld

#endif

