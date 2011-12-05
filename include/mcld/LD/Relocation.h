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
#include <mcld/MC/MCFragmentRef.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/Howto.h>


namespace mcld
{
class ResolveInfo;

class Relocation : public llvm::ilist_node<Relocation>
{
friend class RelocationFactory;
public:
  typedef Howto::Address Address; // FIXME: use SizeTrait<T>::Address instead
  typedef Howto::DWord DWord; // FIXME: use SizeTrait<T>::Word instead
  typedef Howto::Type Type;

private:
  Relocation(const Howto& pHowto,
             const MCFragmentRef& pTargetRef,
             Address pAddend,
	     const ResolveInfo& pSymInfo);
public:

  ~Relocation();
  // -----  observers  ----- //
  /// relocation type
  Type type() const;
  
  /// symbol value - S value
  Address symValue() const
  { return m_pSymInfo->value(); }

  /// addend - A value
  Address addend() const
  { return m_Addend; }
  
  /// place - P value
  Address place() const;

  /// symbol info - binding, type
  const ResolveInfo* symInfo() const
  { return m_pSymInfo; }

  const Howto* howto() const
  { return m_pHowto; }
  
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
  
private:
  /// m_pSymInfo - resolved symbol info of relocation target symbol
  const ResolveInfo* m_pSymInfo;

  /// m_TargetAddress - MCFragmentRef of the place being relocated
  MCFragmentRef m_TargetAddress;
  
  /// m_Target - target data of the place being relocated
  DWord m_Target;
  
  Address m_Addend;
  const Howto* m_pHowto;

};

} // namespace of mcld

#endif

