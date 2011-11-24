//===- LDSymbol.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_SYMBOL_H
#define MCLD_LD_SYMBOL_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/ADT/Uncopyable.h"
#include "llvm/MC/MCAssembler.h"

namespace mcld
{

/** \class LDSymbol
 *  \brief LDSymbol provides a consistent abstraction for different formats
 *  in different targets.
 *
 *  Because LDSymbol is a kind of class that its size is various, LDSymbol
 *  can only be declared in dynamic storage. In order to make sure this,
 *  LDSymbolFactory creates LDSymbols.
 */
class LDSymbol
{
friend class LDSymbolFactory;
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

  // FIXME: use SizeTrait<32> or SizeTrait<64> instead of big type
  typedef uint64_t ValueType;
  typedef uint64_t SizeType;

  typedef llvm::StringRef key_type;

public:
  // -----  observers  ----- //
  const char* name() const
  { return m_String; }

  llvm::StringRef str() const
  { return llvm::StringRef(m_String, m_StrLength); }

  bool isDyn() const
  { return m_IsDyn; }

  unsigned int type() const
  { return m_Type; }

  unsigned int binding() const
  { return m_Binding; }

  const llvm::MCSectionData* section() const
  { return m_pSection; }

  ValueType value() const
  { return m_Value; }

  SizeType size() const
  { return m_Size; }

  uint8_t other() const
  { return m_Other; }

  bool compare(const key_type& pKey);

  // -----  modifiers  ----- //
  void setDynamic(bool pEnable=true)
  { m_IsDyn = pEnable; }

  void setType(unsigned int pType)
  { m_Type = pType; }

  void setBinding(unsigned int pBinding)
  { m_Binding = pBinding; }

  void setSection(const llvm::MCSectionData* pSection)
  { m_pSection = pSection; }

  void setValue(ValueType pValue)
  { m_Value = pValue; }

  void setSize(SizeType pSize)
  { m_Size = pSize; }

  void setOther(uint8_t pOther)
  { m_Other = pOther; }

private:
  LDSymbol();
  LDSymbol(const LDSymbol& pCopy);
  LDSymbol& operator=(const LDSymbol& pCopy);
  ~LDSymbol();

private:
  // -----  Symbol's fields  ----- //
  unsigned int m_Type : 3;
  unsigned int m_Binding : 2;
  bool m_IsDyn : 1;
  uint8_t m_Other;

  const llvm::MCSectionData* m_pSection;
  ValueType m_Value; 
  SizeType m_Size;

  // -----  Symbol and String's fields  ----- //
  size_t m_StrLength;
  char m_String[0];
};

} // namespace mcld

#endif

