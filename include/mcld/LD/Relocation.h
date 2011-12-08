/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#ifndef LD_RELOCATION_H
#define LD_RELOCATION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/ilist_node.h>
#include <llvm/Support/DataTypes.h>
#include <mcld/MC/MCFragmentRef.h>
#include <mcld/LD/ResolveInfo.h>


namespace mcld
{
class Layout;
class ResolveInfo;

class Relocation : public llvm::ilist_node<Relocation>
{
friend class RelocationFactory;

public:
  typedef uint64_t Address; // FIXME: use SizeTrait<T>::Address instead
  typedef uint64_t DWord; // FIXME: use SizeTrait<T>::Word instead
  typedef uint8_t Type;
  typedef void(*Pointer)(Relocation&);

private:
  Relocation(Type pType,
	     Pointer pApply,
             const MCFragmentRef& pTargetRef,
             Address pAddend,
             DWord pTarget);
public:
  ~Relocation();
  
  /// type - relocation type
  Type type() const
  { return m_Type; }
  
  /// symValue - symbol value - S value
  Address symValue() const
  { return m_pSymInfo->value(); }

  /// addend - A value
  Address addend() const
  { return m_Addend; }
  
  /// place - P value
  Address place(Layout& pLayout) const;

  /// symbol info - binding, type
  const ResolveInfo* symInfo() const
  { return m_pSymInfo; }

  /// symbol info - binding, type
  ResolveInfo* symInfo()
  { return m_pSymInfo; }
  
  /// target - the target data to relocate
  DWord& target()
  { return m_Target; }
  
  /// target - the target data to relocate
  const DWord& target() const
  { return m_Target; }

  /// targetRef - the reference of the target data
  MCFragmentRef& targetRef()
  { return m_TargetAddress; }

  /// targetRef - the reference of the target data
  const MCFragmentRef& targetRef() const
  { return m_TargetAddress; }
 
  /// apply - function to apply this relocation
  void apply()
  { m_pApply(*this); }

private:
  /// m_Type - the type of the relocation entries
  Type m_Type;

  /// m_pSymInfo - resolved symbol info of relocation target symbol
  ResolveInfo* m_pSymInfo;

  /// m_TargetAddress - MCFragmentRef of the place being relocated
  MCFragmentRef m_TargetAddress;
  
  /// m_Target - target data of the place being relocated
  DWord m_Target;
  
  /// m_Addend - the addend
  Address m_Addend;

  /// m_pApply - function pointer to the apply function
  Pointer m_pApply;

  /// m_pDynRelocTables - a pointer to hold the DynRelocTables in MCLinker
  // TODO :  
};

} // namespace of mcld

#endif

