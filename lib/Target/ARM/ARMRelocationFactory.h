//===-  ARMRelocationFactory.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ARM_RELOCATION_FACTORY_H
#define ARM_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/LD/RelocationFactory.h"
#include "mcld/Support/GCFactory.h"

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


private:
  /// m_ApplyFuncs - An array to map relocation type to its apply function
  static Pointer m_ApplyFuncs[];

private:
  DWord thumbBit(Relocation &pReloc);
  // Relocation applying function for performing all types of relocations
  void none(Relocation& pReloc);
  void abs32(Relocation& pReloc);
  void rel32(Relocation& pReloc);
  void gotoff32(Relocation& pReloc);
  void got_brel(Relocation& pReloc);
  // TODO: Implement all relocation applying functions
};

} // namespace of mcld

#endif

