//===- MipsRelocationFactory.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_RELOCATION_FACTORY_H
#define MIPS_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/LD/RelocationFactory.h"
#include "mcld/Support/GCFactory.h"

namespace mcld
{

// Each apply function should return its relocation status
enum MipsRelocStatus
{
  STATUS_OK,
  STATUS_OVERFLOW,
  STATUS_BAD_RELOC
};

/** \class MipsRelocationFactory
 *  \brief MipsRelocationFactory creates and destroys the Mips relocations.
 *
 */
class MipsRelocationFactory : public RelocationFactory
{
public:
  typedef MipsRelocStatus (MipsRelocationFactory::*Pointer)(Relocation&);

public:
  MipsRelocationFactory(size_t pNum);
  ~MipsRelocationFactory();

  void apply(Relocation& pRelocation);

private:
  /// m_ApplyFuncs - An array to map relocation type to its apply function
  static Pointer m_ApplyFuncs[];

  // Relocation applying function for performing all types of relocations
#ifdef MIPS_RELOC_FUNC_DECL
# error "MIPS_RELOC_FUNC_DECL should be undefined."
#else
# define MIPS_RELOC_FUNC_DECL(FuncName) \
    MipsRelocStatus FuncName(Relocation& pReloc);
# include "MipsRelocationFunction.def"
#endif
};

} // namespace of mcld

#endif
