/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef ARM_RELOCATION_FACTORY_H
#define ARM_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/RelocationFactory.h>
#include <mcld/Support/GCFactory.h>

namespace mcld
{

/** \class ARMRelocationFactory
 *  \brief ARMRelocationFactory creates and destroys the ARM relocations.
 *
 */
class ARMRelocationFactory : public RelocationFactory, private GCFactory<Relocation, 0>
{
public:
  ARMRelocationFactory(size_t pNum);
  ~ARMRelocationFactory();

  // ----- production ----- //
  Relocation* produce(Type pType,
                      const LDSymbol& pSymbol,
                      MCFragmentRef& pFragRef,
                      DWord* pTargetData = NULL,
                      Address pAddend = 0);

  void destroy(Relocation* pRelocation);
};

} // namespace of mcld

#endif

