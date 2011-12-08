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
class ARMRelocationFactory : public RelocationFactory
{
public:
  typedef void (ARMRelocationFactory::*Pointer)(Relocation&);

public:
  ARMRelocationFactory(size_t pNum);
  ~ARMRelocationFactory();

  void apply(Relocation& pRelocation); 

  void destroy(Relocation* pRelocation);

private:
  /// m_ApplyFuncs - An array to map relocation type to its apply function
  static Pointer m_ApplyFuncs[];

/** list all relocation
  static Relocations[] = {
    
  };

  // clone a relocation
  // set up the relocation
  // return the relocation
**/


};

} // namespace of mcld

#endif

