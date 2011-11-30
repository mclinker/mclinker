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
#include <mcld/Support/FileSystem.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDHowto.h>
#include <mcld/MC/MCFragmentRef.h>

namespace mcld
{
class LDHowto;
class ResolveInfo;
class MCFragmentRef;


class LDRelocation
{
friend class LDRelocationFactory;

private:
  LDRelocation(const LDHowto& pHowto, 
	       MCFragmentRef& pFragmentRef, 
	       uint64_t pAddend,
		   uint32_t pType);

public: 
  ~LDRelocation();

  // -----  observers  ----- //
  const ResolveInfo* symInfo() const;

  uint64_t addend() const;
  
  uint32_t type() const; 
  
  const LDHowto* howto() const;
  
  const MCFragmentRef* framgmentRef() const;
  
  /// place - P value
  uint32_t place() const;

private:
  /// m_pSymInfo - Resolved symbol info of relocation target symbol
  ResolveInfo* m_pSymInfo;             
  /// m_FragmentRef - MCFragmentRef of the place being relocated
  MCFragmentRef m_FragmentRef;         
  uint64_t m_Addend;                           
  uint32_t m_Type;
  const LDHowto* m_pHowto;

};

} // namespace of mcld

#endif

