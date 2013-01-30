//===- HexagonRelocator.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <llvm/Support/DataTypes.h>
#include <llvm/Support/ELF.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Support/MsgHandling.h>

#include "HexagonRelocator.h"
#include "HexagonRelocationFunctions.h"

using namespace mcld;

//===--------------------------------------------------------------------===//
// Relocation Functions and Tables
//===--------------------------------------------------------------------===//
DECL_HEXAGON_APPLY_RELOC_FUNCS

/// the prototype of applying function
typedef Relocator::Result (*ApplyFunctionType)(Relocation& pReloc,
                                               HexagonRelocator& pParent);

// the table entry of applying functions
struct ApplyFunctionTriple
{
  ApplyFunctionType func;
  unsigned int type;
  const char* name;
};

// declare the table of applying functions
static const ApplyFunctionTriple ApplyFunctions[] = {
  DECL_HEXAGON_APPLY_RELOC_FUNC_PTRS
};

//===--------------------------------------------------------------------===//
// HexagonRelocator
//===--------------------------------------------------------------------===//
HexagonRelocator::HexagonRelocator(HexagonLDBackend& pParent)
  : Relocator(),
    m_Target(pParent) {
}

HexagonRelocator::~HexagonRelocator()
{
}

Relocator::Result
HexagonRelocator::applyRelocation(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();

  if (type >= sizeof (ApplyFunctions) / sizeof (ApplyFunctions[0]) ) {
    return Unknown;
  }

  // apply the relocation
  return ApplyFunctions[type].func(pRelocation, *this);
}

const char* HexagonRelocator::getName(Relocation::Type pType) const
{
  return ApplyFunctions[pType].name;
}

//===--------------------------------------------------------------------===//
// Relocation helper function
//===--------------------------------------------------------------------===//





//=========================================//
// Each relocation function implementation //
//=========================================//

// R_HEX_NONE
HexagonRelocator::Result none(Relocation& pReloc, HexagonRelocator& pParent)
{
  return HexagonRelocator::OK;
}

// R_HEX_B22_PCREL: Word32_B22 : 0x01ff3ffe  (S + A - P) >> 2 : Signed Verify
HexagonRelocator::Result relocB22PCREL(Relocation& pReloc, 
                                       HexagonRelocator& pParent)
{
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord   A = pReloc.addend();
  HexagonRelocator::DWord   P = pReloc.place();

  int32_t result = (int32_t) ((S + A - P) >> 2);
  int32_t range = 1 << 21;

  if ( (result < range) && (result > -range)) {
    pReloc.target() = ((pReloc.target()<<1) & 0x3ffe) | 
                      ((pReloc.target()<<3) & 0x01ff0000);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

HexagonRelocator::Result unsupport(Relocation& pReloc, 
                                   HexagonRelocator& pParent)
{
  return HexagonRelocator::Unsupport;
}

