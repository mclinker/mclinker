/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 ****************************************************************************/
#ifndef LD_RELOCATION_FACTORY_H
#define LD_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>
#include <mcld/LD/Relocation.h>

namespace mcld
{

class LDSymbol;
class ResolveInfo;
class MCFragmentRef;

/** \class RelocationFactory
 *  \brief RelocationFactory provides the interface for generating target
 *  relocation
 *
 */
class RelocationFactory : public GCFactory<Relocation, 0>
{
public:
  typedef Relocation::Type Type;
  typedef Relocation::Address Address;
  typedef Relocation::DWord DWord;
  typedef Relocation::Pointer Pointer;

public:
  RelocationFactory(size_t pNum);
  virtual ~RelocationFactory();

  virtual void destroy(Relocation* pRelocation) = 0;
  
  /// getApply - get apply fnuction for relocation type pRelType
  virtual const Pointer getApply(Type pRelType) const = 0;

  // ----- production ----- //
  Relocation* produce(Type pType,
                      MCFragmentRef& pFragRef,
		      DWord pTarget,
                      Address pAddend = 0);
};

} // namespace of mcld

#endif

