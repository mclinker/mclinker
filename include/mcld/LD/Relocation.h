//===- Relocation.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_RELOCATION_H
#define MCLD_LD_RELOCATION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/ilist_node.h>
#include <llvm/Support/DataTypes.h>

#include <mcld/LD/Fragment.h>
#include <mcld/LD/FragmentRef.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDSymbol.h>


namespace mcld
{
class Layout;
class RelocationFactory;
class MCLDInfo;

class Relocation : public Fragment
{
friend class RelocationFactory;

public:
  typedef uint64_t Address; // FIXME: use SizeTrait<T>::Address instead
  typedef uint64_t DWord; // FIXME: use SizeTrait<T>::Word instead
  typedef uint8_t Type;

private:
  Relocation(Type pType,
             FragmentRef* pTargetRef,
             Address pAddend,
             DWord pTargetData);

public:
  ~Relocation();

  /// type - relocation type
  Type type() const
  { return m_Type; }

  /// symValue - S value - the symbol address
  Address symValue() const;

  /// addend - A value
  Address addend() const
  { return m_Addend; }

  /// place - P value - address of the place being relocated
  Address place(const Layout& pLayout) const;

  /// symbol info - binding, type
  const ResolveInfo* symInfo() const
  { return m_pSymInfo; }

  /// symbol info - binding, type
  ResolveInfo* symInfo()
  { return m_pSymInfo; }

  /// target - the target data to relocate
  DWord& target();

  /// target - the target data to relocate
  const DWord& target() const;

  /// targetRef - the reference of the target data
  FragmentRef& targetRef()
  { return m_TargetAddress; }

  /// targetRef - the reference of the target data
  const FragmentRef& targetRef() const
  { return m_TargetAddress; }

  void apply(RelocationFactory& pRelocFactory, const MCLDInfo& pLDInfo);

  /// ----- modifiers ----- ///
  void setType(Type pType);

  void setAddend(Address pAddend);

  void setSymInfo(ResolveInfo* pSym);

  // Relocation is a kind of Fragment with type of FT_Reloc
  static bool classof(const Fragment *F)
  { return F->getKind() == Fragment::Relocation; }
  static bool classof(const Relocation *) { return true; }

private:
  /// m_Type - the type of the relocation entries
  Type m_Type;

  /// m_TargetData - target data of the place being relocated
  DWord m_TargetData;

  /// m_pSymInfo - resolved symbol info of relocation target symbol
  ResolveInfo* m_pSymInfo;

  /// m_TargetAddress - FragmentRef of the place being relocated
  FragmentRef m_TargetAddress;

  /// m_Addend - the addend
  Address m_Addend;
};

} // namespace of mcld

#endif

