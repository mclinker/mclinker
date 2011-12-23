//===- ARMRelocationFactory.cpp  ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <string>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include "ARMRelocationFactory.h"
#include "ARMRelocationFunctions.h"

using namespace mcld;

DECL_ARM_APPLY_RELOC_FUNCS

//===----------------------------------------------------------------------===//
// ARMRelocationFactory
ARMRelocationFactory::ARMRelocationFactory(size_t pNum, ARMGNULDBackend& pParent)
  : RelocationFactory(pNum),
    m_Target(pParent) {
}

ARMRelocationFactory::~ARMRelocationFactory()
{
}

void ARMRelocationFactory::applyRelocation(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();
  if (type > 130) { // 131-255 doesn't noted in ARM spec
    llvm::report_fatal_error(llvm::Twine("Unknown relocation type. To symbol `") +
                             pRelocation.symInfo()->name() +
                             llvm::Twine("'."));
    return;
  }

  /// the prototype of applying function
  typedef Result (*ApplyFunctionType)(Relocation& pReloc, const ARMRelocationFactory& pParent);

  // the table entry of applying functions
  struct ApplyFunctionTriple {
    ApplyFunctionType func;
    unsigned int type;
    const char* name;
  };

  // declare the table of applying functions
  static ApplyFunctionTriple apply_functions[] = {
    DECL_ARM_APPLY_RELOC_FUNC_PTRS
  };

  // apply the relocation 
  Result result = apply_functions[type].func(pRelocation, *this);

  // check result
  if (Overflow == result) {
    llvm::report_fatal_error(llvm::Twine("Applying relocation `") +
                             llvm::Twine(apply_functions[type].name) +
                             llvm::Twine("' causes overflow. on symbol: `") +
                             llvm::Twine(pRelocation.symInfo()->name()) +
                             llvm::Twine("'."));
    return;
  }

  if (BadReloc == result) {
    llvm::report_fatal_error(llvm::Twine("Applying relocation `") +
                             llvm::Twine(apply_functions[type].name) +
                             llvm::Twine("' encounters unexpected opcode. on symbol: `") +
                             llvm::Twine(pRelocation.symInfo()->name()) +
                             llvm::Twine("'."));
    return;
  }
}

//===----------------------------------------------------------------------===//
// non-member functions
static RelocationFactory::DWord getThumbBit(const Relocation& pReloc)
{
  // Set thumb bit if
  // - symbol has type of STT_FUNC, is defined and with bit 0 of its value set
  RelocationFactory::DWord thumbBit = 
       ((pReloc.symInfo()->desc() != ResolveInfo::Undefined) && 
        (pReloc.symInfo()->type() == ResolveInfo::Function) &&
        ((pReloc.symValue() & 0x1) != 0))?
        1:0;
  return thumbBit;
}


//=========================================//
// Each relocation function implementation //
//=========================================//

// R_ARM_NONE
ARMRelocationFactory::Result none(Relocation& pReloc, const ARMRelocationFactory& pParent)
{
  return ARMRelocationFactory::OK;
}

// R_ARM_ABS32: (S + A) | T
ARMRelocationFactory::Result abs32(Relocation& pReloc, const ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord t_bit = getThumbBit(pReloc);
  ARMRelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = (pReloc.symValue() + addend) | t_bit;
  return ARMRelocationFactory::OK;
}

// R_ARM_REL32: ((S + A) | T) - P
ARMRelocationFactory::Result rel32(Relocation& pReloc, const ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord t_bit = getThumbBit(pReloc);
  ARMRelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + addend) | t_bit)
                    - pReloc.place(pParent.getLayout());
  return ARMRelocationFactory::OK;
}

// R_ARM_GOTOFF32: ((S + A) | T) - GOT_ORG
ARMRelocationFactory::Result gotoff32(Relocation& pReloc, const ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord t_bit = getThumbBit(pReloc);
  ARMRelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
  ARMRelocationFactory::Address got_addr = static_cast<const LDSection&>
    (pParent.getTarget().getGOT().getSectionData()->getSection()).offset();

  pReloc.target() = ((pReloc.symValue() + addend) | t_bit) - got_addr;
  return ARMRelocationFactory::OK;
}

