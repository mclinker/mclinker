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

#include <llvm/Support/DataTypes.h>
#include <llvm/ADT/StringRef.h>

namespace mcld
{

/** \class ResolveInfo
 *  \brief ResolveInfo records the information about how to resolve a symbol.
 *
 *  A symbol must have some `attributes':
 *  - Desc - Defined, Reference, Common or Indirect
 *  - Binding - Global, Local, Weak
 *  - IsDyn - appear in dynamic objects or regular objects
 *  - Type - what the symbol points to
 *  - Size  - the size of the symbol point to
 *  - Value - the value of the symbol
 *  In order to save the memory and speed up the performance, MCLinker uses
 *  a bit field to store all attributes.
 *
 *  The maximum string length is (2^16 - 1)
 */
class ResolveInfo
{
friend class ResolveInfoFactory;
public:
  typedef uint64_t SizeType;
  typedef uint64_t ValueType;

  /** \enum Type
   *  \brief What the symbol stand for
   *
   *  It is like ELF32_ST_TYPE
   *  MachO does not need this, and can not jump between Thumb and ARM code.
   */
  enum Type {
    NoType   = 0,
    Object   = 1,
    Function = 2,
    Section  = 3,
    File     = 4,
    LoProc   = 13,
    HiProc   = 15
  };

  /** \enum Desc
   *  \brief Description of the symbols.
   *
   *   Follow the naming in MachO. Like MachO nlist::n_desc
   *   In ELF, is a part of st_shndx
   */
  enum Desc {
    Undefined,
    Define,
    Common,
    Indirect,
    NoneDesc
  };

  enum Binding {
    Global,
    Weak,
    Local,
    Absolute,
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
  /// setRegular - set the source of the file is a regular object
  void setRegular();

  /// setDynamic - set the source of the file is a dynamic object
  void setDynamic();

  /// setSource - set the source of the file
  /// @param pIsDyn is the source from a dynamic object?
  void setSource(bool pIsDyn);

  void setType(uint32_t pType);

  void setDesc(uint32_t pDesc);

  void setBinding(uint32_t pBinding);

  void setOther(uint32_t pOther);

  void setVisibility(Visibility pVisibility);

  void setHasPLT(bool pHasPLT);

  void setIsSymbol(bool pIsSymbol);

  void setValue(ValueType pValue)
  { m_Value.value = pValue; }

  void setSize(SizeType pSize)
  { m_Size = pSize; }

  void override(const ResolveInfo& pForm);

  void overrideAttributes(const ResolveInfo& pFrom);

  void overrideVisibility(const ResolveInfo& pFrom);

  void setLink(const ResolveInfo* pTarget) {
    m_Value.ptr = const_cast<ResolveInfo*>(pTarget);
    m_BitField |= indirect_flag;
  }


  // -----  observers  ----- //
  bool isSymbol() const;

  bool isGlobal() const;

  bool isWeak() const;

  bool isLocal() const;

  bool isAbsolute() const;

  bool isDefine() const;

  bool isUndef() const;

  bool isDyn() const;

  bool isCommon() const;

  bool isIndirect() const;

  bool hasPLT() const;

  unsigned int type() const;

  unsigned int desc() const;

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
  static const uint32_t GLOBAL_OFFSET = 0;
  static const uint32_t GLOBAL_MASK = 1;

  static const uint32_t DYN_OFFSET = 1;
  static const uint32_t DYN_MASK = 1 << DYN_OFFSET;

  static const uint32_t DESC_OFFSET = 2;
  static const uint32_t DESC_MASK = 0x3 << DESC_OFFSET;

  static const uint32_t LOCAL_OFFSET = 4;
  static const uint32_t LOCAL_MASK = 1 << LOCAL_OFFSET;

  static const uint32_t BINDING_MASK = GLOBAL_MASK | LOCAL_MASK;

  static const uint32_t VISIBILITY_OFFSET = 5;
  static const uint32_t VISIBILITY_MASK = 0x3 << VISIBILITY_OFFSET;

  static const uint32_t PLT_OFFSET = 7;
  static const uint32_t PLT_MASK = 1 << PLT_OFFSET;

  static const uint32_t TYPE_OFFSET = 8;
  static const uint32_t TYPE_MASK = 0xF << TYPE_OFFSET;

  static const uint32_t ISSYMBOL_OFFSET = 12;
  static const uint32_t ISSYMBOL_MASK = 1 << ISSYMBOL_OFFSET;

  static const uint32_t NAME_LENGTH_OFFSET = 16;
  static const uint32_t RESOLVE_MASK = 0xFFF;

  union ValOrPtr {
    ValueType value;
    ResolveInfo* ptr;
  };

public:
  static const uint32_t global_flag    = 0        << GLOBAL_OFFSET;
  static const uint32_t weak_flag      = 1        << GLOBAL_OFFSET;
  static const uint32_t regular_flag   = 0        << DYN_OFFSET;
  static const uint32_t dynamic_flag   = 1        << DYN_OFFSET;
  static const uint32_t undefine_flag  = 0        << DESC_OFFSET;
  static const uint32_t define_flag    = 1        << DESC_OFFSET;
  static const uint32_t common_flag    = 2        << DESC_OFFSET;
  static const uint32_t indirect_flag  = 3        << DESC_OFFSET;
  static const uint32_t local_flag     = 1        << LOCAL_OFFSET;
  static const uint32_t absolute_flag  = BINDING_MASK;
  static const uint32_t has_plt_flag   = 1        << PLT_OFFSET;
  static const uint32_t object_flag    = Object   << TYPE_OFFSET;
  static const uint32_t function_flag  = Function << TYPE_OFFSET;
  static const uint32_t section_flag   = Section  << TYPE_OFFSET;
  static const uint32_t file_flag      = File     << TYPE_OFFSET;
  static const uint32_t is_symbol_flag = 1        << ISSYMBOL_OFFSET;

private:
  ResolveInfo();
  ResolveInfo(const ResolveInfo& pCopy);
  ResolveInfo& operator=(const ResolveInfo& pCopy);
  ~ResolveInfo();

private:
  SizeType m_Size;
  ValOrPtr m_Value;

  /** m_BitField
   *  31     ...    16 15    13 12     11..8 7   6      ..    5 4     3   2   1   0
   * |length of m_Name|reserved|Symbol|Type |PLT|ELF visibility|Local|Com|Def|Dyn|Weak|
   */
  uint32_t m_BitField;
  char m_Name[0];
};

} // namespace of mcld

#endif

