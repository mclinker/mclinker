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
#include <mcld/MC/MCFragmentRef.h>
#include <mcld/LD/Howto.h>

namespace mcld
{
class ResolveInfo;

class Relocation
{
friend class RelocationFactory;
public:
  typedef Howto::Address Address; // FIXME: use SizeTrait<T>::Address instead
  typedef Howto::Type Type;

private:
  Relocation(const Howto& pHowto,
             const MCFragmentRef& pTarget,
             Address pAddend);

  ~Relocation();

public:
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
  
  /// target - the target address to relocate
  Address target()
  { return m_Target; }
  
  /// target - the target address to relocate
  const MCFragmentRef& target() const
  { return m_Target; }
  
private:
  /// m_pSymInfo - Resolved symbol info of relocation target symbol
  ResolveInfo* m_pSymInfo;

  /// m_Target - MCFragmentRef of the place being relocated
  MCFragmentRef m_Target;
  Address m_Addend;
  const Howto* m_pHowto;

};

} // namespace of mcld

#endif

