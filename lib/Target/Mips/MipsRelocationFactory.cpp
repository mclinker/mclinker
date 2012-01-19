//===- MipsRelocationFactory.cpp  -----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/ErrorHandling.h>

#include "MipsRelocationFactory.h"
#include "MipsRelocationFunctions.h"

using namespace mcld;

DECL_MIPS_APPLY_RELOC_FUNCS

//==========================
// MipsRelocationFactory
MipsRelocationFactory::MipsRelocationFactory(size_t pNum,
                                             MipsGNULDBackend& pParent)
  : RelocationFactory(pNum),
    m_Target(pParent)
{
}

void MipsRelocationFactory::applyRelocation(Relocation& pRelocation,
                                            const MCLDInfo& pLDInfo)

{
  /// the prototype of applying function
  typedef Result (*ApplyFunctionType)(Relocation&,
                                      const MipsRelocationFactory&);

  // the table entry of applying functions
  struct ApplyFunctionTriple {
    ApplyFunctionType func;
    unsigned int type;
    const char* name;
  };

  // declare the table of applying functions
  static ApplyFunctionTriple apply_functions[] = {
    DECL_MIPS_APPLY_RELOC_FUNC_PTRS
  };

  Relocation::Type type = pRelocation.type();

  if (type >= sizeof(apply_functions) / sizeof(apply_functions[0])) {
    llvm::report_fatal_error(llvm::Twine("Unknown relocation type. "
                                         "To symbol `") +
                             pRelocation.symInfo()->name() +
                             llvm::Twine("'."));
  }

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
                             llvm::Twine("' encounters unexpected opcode. "
                                         "on symbol: `") +
                             llvm::Twine(pRelocation.symInfo()->name()) +
                             llvm::Twine("'."));
    return;
  }
}

//=========================================//
// Relocation functions implementation     //
//=========================================//

static const char * const GP_DISP_NAME = "_gp_disp";

// R_MIPS_NONE and those unsupported/deprecated relocation type
MipsRelocationFactory::Result
none(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  return MipsRelocationFactory::OK;
}

// R_MIPS_32: S + A
MipsRelocationFactory::Result
abs32(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  RelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = pReloc.symValue() + addend;
  return MipsRelocationFactory::OK;
}

// R_MIPS_REL32: A - EA + S
MipsRelocationFactory::Result
rel32(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  RelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = addend - pReloc.place(pParent.getLayout())
                           + pReloc.symValue();
  return MipsRelocationFactory::OK;
}

// R_MIPS_HI16:
//   local/external: ((AHL + S) - (short)(AHL + S)) >> 16
//   _gp_disp      : ((AHL + GP - P) - (short)(AHL + GP - P)) >> 16
MipsRelocationFactory::Result
hi16(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  // Nothing to do. Process this relocation in the 'lo16' routine.
  return MipsRelocationFactory::OK;
}

// R_MIPS_LO16:
//   local/external: AHL + S
//   _gp_disp      : AHL + GP - P + 4
MipsRelocationFactory::Result
lo16(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  // TODO (simon): Consider to support GNU extension -
  // multiple R_MIPS_HI16 entries for single R_MIPS_LO16.

  Relocation *hiReloc = static_cast<Relocation*>(pReloc.getPrevNode());

  if (strcmp(pReloc.symInfo()->name(), GP_DISP_NAME) == 0) {
    // TODO (simon): Implement relocation for _gp_disp
  } else {
    RelocationFactory::DWord loPart = pReloc.addend() & 0xffff;

    if (hiReloc != 0 && hiReloc->type() == llvm::ELF::R_MIPS_HI16) {
        RelocationFactory::DWord hiPart = hiReloc->addend() +
            ((loPart + 0x8000) & 0xffff);

        RelocationFactory::DWord hiAddend = hiReloc->target() + hiPart;
        hiReloc->target() = (hiReloc->symValue() + hiAddend) >> 16;
    }

    RelocationFactory::DWord addend = pReloc.target() + loPart;
    pReloc.target() = pReloc.symValue() + addend;
  }

  return MipsRelocationFactory::OK;
}

// R_MIPS_GOT16:
//   local   : tbd
//   external: G
MipsRelocationFactory::Result
got16(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  return MipsRelocationFactory::OK;
}

// R_MIPS_CALL16: G
MipsRelocationFactory::Result
call16(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  return MipsRelocationFactory::OK;
}

// R_MIPS_GPREL32: A + S + GP0 - GP
MipsRelocationFactory::Result
gprel32(Relocation& pReloc, const MipsRelocationFactory& pParent)
{
  return MipsRelocationFactory::OK;
}
