//===- ARMRelocationFactory.cpp  ----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//

#include <string>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <mcld/LD/Layout.h>
#include "ARMRelocationFactory.h"
#include "ARMRelocationFunctions.h"
#include <stdint.h>

using namespace mcld;

DECL_ARM_APPLY_RELOC_FUNCS

//===--------------------------------------------------------------------===//
// ARMRelocationFactory
ARMRelocationFactory::ARMRelocationFactory(size_t pNum,
                                           ARMGNULDBackend& pParent)
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
    llvm::report_fatal_error(llvm::Twine("Unknown relocation type. "
                                         "To symbol `") +
                             pRelocation.symInfo()->name() +
                             llvm::Twine("'."));
    return;
  }

  /// the prototype of applying function
  typedef Result (*ApplyFunctionType)(Relocation& pReloc,
                                      ARMRelocationFactory& pParent);

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
                             llvm::Twine("' encounters unexpected opcode. "
                                         "on symbol: `") +
                             llvm::Twine(pRelocation.symInfo()->name()) +
                             llvm::Twine("'."));
    return;
  }
}



//===--------------------------------------------------------------------===//
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
// Relocation helper function              //
//=========================================//

static
ARMRelocationFactory::Address helper_GOT_ORG(ARMRelocationFactory& pParent)
{
  const LDSection& ld_section = static_cast<const LDSection&>(
    pParent.getTarget().getGOT().getSectionData()->getSection()
  );
  return ld_section.offset();
}


static
ARMRelocationFactory::Address helper_GOT(ARMRelocationFactory& pParent,
                                         const GOTEntry& pGOTEntry)
{
  return helper_GOT_ORG(pParent) +
         pParent.getLayout().getFragmentOffset(pGOTEntry);
}


static
ARMRelocationFactory::Address helper_PLT_ORG(ARMRelocationFactory& pParent)
{
  const LDSection& ld_section = static_cast<const LDSection&>(
    // XXX: Why this getSectionData is reference, but GOT is not.
    pParent.getTarget().getPLT().getSectionData().getSection()
  );
  return ld_section.offset();
}


static
ARMRelocationFactory::Address helper_PLT(ARMRelocationFactory& pParent,
                                         const PLTEntry& pPLTEntry)
{
  return helper_PLT_ORG(pParent) +
         pParent.getLayout().getFragmentOffset(pPLTEntry);
}


// Using uint64_t to make sure those complicate operations won't cause
// undefined behavior.
static
uint64_t helper_sign_extend(uint64_t pVal, uint64_t pOri_width)
{
  assert(pOri_width <= 64);
  uint64_t sign_bit = 1 << (pOri_width - 1);
  return (pVal ^ sign_bit) - sign_bit;
  // Reverse sign bit, then subtract sign bit.
}


static
GOTEntry& helper_get_GOT_and_init(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  ARMGNULDBackend& ld_backend = pParent.getTarget();

  bool exist;
  GOTEntry& got_entry = *ld_backend.getGOT().getEntry(*rsym, exist);
  if (!exist) {
    if (rsym->reserved() & ARMGNULDBackend::ReserveGOT) {
      // No corresponding dynamic relocation, initialize to the symbol value.
      got_entry.setContent(pReloc.symValue());
    }
    else if (rsym->reserved() & ARMGNULDBackend::GOTRel) {
      // Initialize to 0 for corresponding dynamic relocation.
      got_entry.setContent(0);

      // Initialize corresponding dynamic relocation.
      Relocation* rel_entry = ld_backend.getRelDyn().getEntry(*rsym, exist);
      if (exist) {
        llvm::report_fatal_error("Don't exist GOT, but exist DynRel!");
      }
      rel_entry->setType(R_ARM_GLOB_DAT);
      rel_entry->targetRef().assign(got_entry);
      rel_entry->setSymInfo(rsym);
    }
    else {
      llvm::report_fatal_error("Didn't reserve GOT!");
    }
  }
  return got_entry;
}




//=========================================//
// Each relocation function implementation //
//=========================================//

// R_ARM_NONE
ARMRelocationFactory::Result none(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  return ARMRelocationFactory::OK;
}

// R_ARM_ABS32: (S + A) | T
ARMRelocationFactory::Result abs32(Relocation& pReloc,
                                   ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord t_bit = getThumbBit(pReloc);
  ARMRelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = (pReloc.symValue() + addend) | t_bit;
  return ARMRelocationFactory::OK;
}

// R_ARM_REL32: ((S + A) | T) - P
ARMRelocationFactory::Result rel32(Relocation& pReloc,
                                   ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord t_bit = getThumbBit(pReloc);
  ARMRelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + addend) | t_bit)
                    - pReloc.place(pParent.getLayout());
  return ARMRelocationFactory::OK;
}

// R_ARM_GOTOFF32: ((S + A) | T) - GOT_ORG
ARMRelocationFactory::Result gotoff32(Relocation& pReloc,
                                      ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord t_bit = getThumbBit(pReloc);
  ARMRelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
  ARMRelocationFactory::Address got_addr = helper_GOT_ORG(pParent);

  pReloc.target() = ((pReloc.symValue() + addend) | t_bit) - got_addr;
  return ARMRelocationFactory::OK;
}

// R_ARM_GOT_BREL: GOT(S) + A - GOT_ORG
ARMRelocationFactory::Result got_brel(Relocation& pReloc,
                                      ARMRelocationFactory& pParent)
{
  switch (pReloc.symInfo()->reserved()) {
  default:
    return ARMRelocationFactory::BadReloc;

  // Only allow this two reserve entry type.
  case ARMGNULDBackend::ReserveGOT:
  case ARMGNULDBackend::GOTRel:
    GOTEntry& got_entry = helper_get_GOT_and_init(pReloc, pParent);
    // Get addend.
    ARMRelocationFactory::DWord addend = pReloc.target() + pReloc.addend();
    // Apply relocation.
    pReloc.target() = helper_GOT(pParent, got_entry) +
                      addend -
                      helper_GOT_ORG(pParent);
    return ARMRelocationFactory::OK;
  }
}

// R_ARM_PLT32: ((S + A) | T) - P
ARMRelocationFactory::Result plt32(Relocation& pReloc,
                                   ARMRelocationFactory& pParent)
{
  return call(pReloc, pParent);
}

// R_ARM_JUMP24: ((S + A) | T) - P
ARMRelocationFactory::Result jump24(Relocation& pReloc,
                                    ARMRelocationFactory& pParent)
{
  return call(pReloc, pParent);
}

// R_ARM_CALL: ((S + A) | T) - P
ARMRelocationFactory::Result call(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  // TODO: Some issue have not been considered, e.g. thumb, overflow?

  ARMRelocationFactory::Address S;
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A =
    helper_sign_extend((pReloc.target() & 0x00FFFFFFu), 24) + pReloc.addend();
  ARMRelocationFactory::Address P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord X;

  pReloc.target() &= 0xFF000000u;

  switch (pReloc.symInfo()->reserved()) {
  default:
    return ARMRelocationFactory::BadReloc;

  case ARMGNULDBackend::None:
    S = pReloc.symValue();
    break;
  case ARMGNULDBackend::ReservePLT:
    T = 0;
    // TODO: Dynamic relocation for got.plt..
    // TODO: Write a helper_get_PLT_and_init().
    bool exist;
    PLTEntry& plt_entry = *pParent.getTarget()
                                  .getPLT()
                                  .getEntry(*pReloc.symInfo(), exist);
    P = helper_PLT(pParent, plt_entry);
    break;
  }
  X = ((S + A) | T) - P;
  X >>= 2;
  X &= 0xFFFFFFFFu;
  if (X > 0x007FFFFFu && X < 0xFF800000u) {  // Check X is 24bit sign interger.
    llvm::report_fatal_error("Jump or Call target too far!");
  }
  pReloc.target() |= (X & 0x00FFFFFFu);
  return ARMRelocationFactory::OK;
}

// R_ARM_MOVW_ABS_NC: (S + A) | T
// R_ARM_MOVW_PREL_NC: ((S + A) | T) - P
// R_ARM_THM_MOVW_ABS_NC: (S + A) | T
// R_ARM_THM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocationFactory::Result movw_nc(Relocation& pReloc,
                                     ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A =
      helper_sign_extend((pReloc.target() & 0xffffUL), 16); // initial addend
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord S;

  pReloc.target() &= 0xffff0000UL;

  if (pReloc.type() == R_ARM_MOVW_ABS_NC ||
      pReloc.type() == R_ARM_THM_MOVW_ABS_NC) {
    S = (pReloc.target() + A) | T;
  } else {    // pc-relative
    S = ((pReloc.target() + A) | T) - P;
  }

  if (pReloc.type() == R_ARM_MOVW_ABS_NC ||
      pReloc.type() == R_ARM_MOVW_PREL_NC) {
    S &= 0xffffUL;
  } else {    // thumb-32
    S &= 0x0000ffffUL;
  }

  pReloc.target() |= S;
  return ARMRelocationFactory::OK;
}

// R_ARM_TLS_GD32: GOT(S) + A - P
// R_ARM_TLS_IE32: GOT(S) + A - P
// R_ARM_TLS_LE32: S + A - tp
ARMRelocationFactory::Result tls(Relocation& pReloc,
                                 ARMRelocationFactory& pParent)
{
  llvm::report_fatal_error("We don't support TLS relocation yet.");
  return ARMRelocationFactory::OK;
}
