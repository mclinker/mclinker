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
#include <mcld/LD/Howto.h>
#include <mcld/LD/LDSymbol.h>

namespace mcld
{

class LDHowto;
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
  typedef Howto::Type Type;
  typedef Howto::Address Address;

public:
  RelocationFactory(size_t pNum);
  virtual ~RelocationFactory();
  
  // ----- production ----- //
  Relocation* produce(Type pType,
                      const LDSymbol& pSymbol,
                      Address pOffset,
                      Address pAddend = 0);

  void destroy(Relocation* pRelocation);
};

} // namespace of mcld

#endif

