/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#ifndef LDRELOCATIONFACTORY_H
#define LDRELOCATIONFACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDHowto.h>
#include <mcld/MC/MCFragmentRef.h>

namespace mcld
{

class LDHowto;
class ResolveInfo;
class MCFragmentRef;

/** \class LDRelocationFactory
 *  \brief LDRelocationFactory for producing LDRelocation
 *
 *  \see
 *  \author Diana Chen <diana.chen@mediatek.com>
 */
class LDRelocationFactory : public GCFactory<LDRelocation, 0>
{
public:
  typedef GCFactory<LDRelocation, 0> Alloc;

public:
  LDRelocationFactory(size_t pNum);
  ~LDRelocationFactory();
  
  // ----- production ----- //
  LDRelocation* produce(const LDHowto& pHowto, 
	       MCFragmentRef& pFragmentRef, 
	       uint64_t pAddend,
		   uint32_t pType); 
};

} // namespace of mcld

#endif

