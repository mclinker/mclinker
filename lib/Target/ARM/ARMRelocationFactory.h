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
enum ARMRelocStatus
{
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
  typedef ARMRelocStatus (ARMRelocationFactory::*Pointer)(Relocation&);

public:
  ARMRelocationFactory(size_t pNum);
  ~ARMRelocationFactory();

  void apply(Relocation& pRelocation);


private:
  /// m_ApplyFuncs - An array to map relocation type to its apply function
  static Pointer m_ApplyFuncs[];

private:
  DWord getThumbBit(Relocation &pReloc);
  // Relocation applying function for performing all types of relocations
#ifdef ARM_RELOC_FUNC_DECL
# error "ARM_RELOC_FUNC_DECL should be undefined."
#else
# define ARM_RELOC_FUNC_DECL(FuncName) \
    ARMRelocStatus FuncName(Relocation& pReloc);
# include "ARMRelocationFunction.def"
#endif
};

} // namespace of mcld

#endif

