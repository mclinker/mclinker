/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com> (owner)                                  *
 *   Nowar Gu <nowar100@gmail.com>  (early prototype)                        *
 *   Luba Tang <luba.tang@mediatek.com> (consistent prototype with proposal) *
 ****************************************************************************/
#ifndef MCLD_LD_SYMBOL_H
#define MCLD_LD_SYMBOL_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/Uncopyable.h>
#include <llvm/MC/MCAssembler.h>

namespace mcld
{

/** \class LDSymbol
 *  \brief LDSymbol provides a consistent abstraction for different formats
 *  in different targets.
 */
class LDSymbol : private Uncopyable
{
friend class SymbolTableEntry;
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

private:
  LDSymbol();
  ~LDSymbol();

public:
  // -----  observers  ----- //
  const char* name() const
  { return m_pName; }

  bool isDyn() const
  { return m_IsDyn; }

  Type type() const
  { return m_Type; }

  Binding binding() const
  { return m_Binding; }

  const llvm::MCSectionData* section() const
  { return m_pSection; }

  uint64_t value() const
  { return m_Value; }

  uint64_t size() const
  { return m_Size; }

  uint8_t other() const
  { return m_Other; }

  // -----  modifiers  ----- //
  // setName - direct the string of name to the address in the string table.
  // LDSymbol doest not store the string of the name. Instead, string table
  // does it for LDSymbol. Thus, this function only redirect the string of name.
  void setName(const char* pCString)
  { m_pName = pCString; }
  
  void setDynamic(bool pEnable=true)
  { m_IsDyn = pEnable; }

  void setType(Type pType)
  { m_Type = pType; }

  void setBinding(Binding pBinding)
  { m_Binding = pBinding; }

  void setSection(const llvm::MCSectionData* pSection)
  { m_pSection = pSection; }

  void setValue(uint64_t pValue)
  { m_Value = pValue; }

  void setSize(uint64_t pSize)
  { m_Size = pSize; }

  void setOther(uint8_t pOther)
  { m_Other = pOther; }

private:
  const char* m_pName;
  bool m_IsDyn : 1;
  Type m_Type;
  Binding m_Binding;
  const llvm::MCSectionData* m_pSection;
  uint64_t m_Value;
  uint64_t m_Size;
  uint8_t m_Other;
};

} // namespace mcld

#endif
