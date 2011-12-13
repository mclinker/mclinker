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

// Each apply function should return its relocation status
enum ARM_Reloc_Status {
  STATUS_OK,
  STATUS_OVERFLOW,
  STATUS_BAD_RELOC
};

/** \class ARMRelocationFactory
 *  \brief ARMRelocationFactory creates and destroys the ARM relocations.
 *
 */
class ARMRelocationFactory : public RelocationFactory
{
public:
  typedef ARM_Reloc_Status (ARMRelocationFactory::*Pointer)(Relocation&);

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
  ARM_Reloc_Status none(Relocation& pReloc);
  ARM_Reloc_Status abs32(Relocation& pReloc);
  ARM_Reloc_Status rel32(Relocation& pReloc);
  ARM_Reloc_Status gotoff32(Relocation& pReloc);
  ARM_Reloc_Status got_brel(Relocation& pReloc);
  // TODO: Implement all relocation applying functions
};

} // namespace of mcld

#endif

