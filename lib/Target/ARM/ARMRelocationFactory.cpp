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
    // If we first get this GOT entry, we should initialize it.
    if (rsym->reserved() & ARMGNULDBackend::ReserveGOT) {
      // No corresponding dynamic relocation, initialize to the symbol value.
      got_entry.setContent(pReloc.symValue());
    }
    else if (rsym->reserved() & ARMGNULDBackend::GOTRel) {
      // Initialize to 0 for corresponding dynamic relocation.
      got_entry.setContent(0);

      // Initialize corresponding dynamic relocation.
      Relocation* rel_entry = ld_backend.getRelDyn().getEntry(*rsym, exist);
      assert(!exist && "Don't exist GOT, but exist DynRel!");
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


static
ARMRelocationFactory::Address helper_GOT_ORG(ARMRelocationFactory& pParent)
{
  const LDSection& ld_section = static_cast<const LDSection&>(
    pParent.getTarget().getGOT().getSectionData()->getSection()
  );
  return ld_section.offset();
}


static
ARMRelocationFactory::Address helper_GOT(Relocation& pReloc,
                                         ARMRelocationFactory& pParent)
{
  GOTEntry& got_entry = helper_get_GOT_and_init(pReloc, pParent);
  return helper_GOT_ORG(pParent) +
         pParent.getLayout().getFragmentOffset(got_entry);
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
ARMRelocationFactory::Address helper_PLT(Relocation& pReloc,
                                         ARMRelocationFactory& pParent)
{
  // TODO: Dynamic relocation for got.plt..
  // TODO: Write a helper_get_PLT_and_init().
  bool exist;
  PLTEntry& plt_entry = *pParent.getTarget()
                                .getPLT()
                                .getEntry(*pReloc.symInfo(), exist);
  return helper_PLT_ORG(pParent) +
         pParent.getLayout().getFragmentOffset(plt_entry);
}


static ARMRelocationFactory::DWord
helper_extract_movw_movt_addend(ARMRelocationFactory::DWord target)
{
  // imm16: [19-16][11-0]
  return helper_sign_extend((((target >> 4)) & 0xf000U) | (target & 0xfffU),
                            16);
}

static ARMRelocationFactory::DWord
helper_insert_val_movw_movt_inst(ARMRelocationFactory::DWord target,
                                 ARMRelocationFactory::DWord imm)
{
  // imm16: [19-16][11-0]
  target &= 0xfff0f000U;
  target |= imm & 0x0fffU;
  target |= (imm & 0xf000U) << 4;
  return target;
}

static ARMRelocationFactory::DWord
helper_extract_thumb_movw_movt_addend(ARMRelocationFactory::DWord target)
{
  // imm16: [16-19][26][14-12][7-0]
  return helper_sign_extend((((target >> 4) & 0xf000U) |
                             ((target >> 15) & 0x0800U) |
                             ((target >> 4) & 0x0700U) |
                             (target & 0x00ffU)),
                            16);
}

static ARMRelocationFactory::DWord
helper_insert_val_thumb_movw_movt_inst(ARMRelocationFactory::DWord target,
                                       ARMRelocationFactory::DWord imm)
{
  // imm16: [16-19][26][14-12][7-0]
  target &= 0xfbf08f00U;
  target |= (imm & 0xf000U) << 4;
  target |= (imm & 0x0800U) << 15;
  target |= (imm & 0x0700U) << 4;
  target |= (imm & 0x00ffU);
  return target;
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
    ARMRelocationFactory::Address GOT_S   = helper_GOT(pReloc, pParent);
    ARMRelocationFactory::DWord   A       = pReloc.target() + pReloc.addend();
    ARMRelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
    // Apply relocation.
    pReloc.target() = GOT_S + A - GOT_ORG;
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

  ARMRelocationFactory::Address S; // S dependent on exist PLT or not.
  ARMRelocationFactory::DWord   T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord   A =
    helper_sign_extend((pReloc.target() & 0x00FFFFFFu), 24) + pReloc.addend();
  ARMRelocationFactory::Address P = pReloc.place(pParent.getLayout());

  switch (pReloc.symInfo()->reserved()) {
  default:
    return ARMRelocationFactory::BadReloc;

  case ARMGNULDBackend::None:
    S = pReloc.symValue();
    break;
  case ARMGNULDBackend::ReservePLT:
    S = helper_PLT(pReloc, pParent);
    T = 0;  // PLT is not thumb.
    break;
  }

  ARMRelocationFactory::DWord X = ((S + A) | T) - P;

  if (X & 0x03u) {  // Lowest two bit is not zero.
    llvm::report_fatal_error("Target is thumb, need stub!");
  }
  // Check X is 24bit sign int. If not, we should use stub or PLT before apply.
  assert((X & 0xFFFFFFFFu) > 0x01FFFFFFu &&
         (X & 0xFFFFFFFFu) < 0xFE000000u && "Jump or Call target too far!");
  //                    Make sure the Imm is 0.          Result Mask.
  pReloc.target() = (pReloc.target() & 0xFF000000u) | ((X & 0x03FFFFFEu) >> 2);
  return ARMRelocationFactory::OK;
}

// R_ARM_MOVW_ABS_NC: (S + A) | T
// R_ARM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocationFactory::Result movw(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_movw_movt_addend(pReloc.target());
  ARMRelocationFactory::DWord X;

  if (pReloc.type() == R_ARM_MOVW_ABS_NC) {
    X = (S + A) | T;
  } else {
    X = ((S + A) | T) - P;
  }

  pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
  return ARMRelocationFactory::OK;
}

// R_ARM_MOVT_ABS: S + A
// R_ARM_MOVT_PREL: S + A - P
ARMRelocationFactory::Result movt(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_movw_movt_addend(pReloc.target());
  ARMRelocationFactory::DWord X;

  if (pReloc.type() == R_ARM_MOVT_ABS) {
    X = S + A;
  } else {
    X = S + A - P;
  }

  X >>= 16;

  pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);

  // check 16-bit overflow
  int32_t max = (1 << (16 - 1)) - 1;
  int32_t min = -(1 << (16 - 1));
  int32_t signed_x = static_cast<int32_t>(X);
  if (signed_x > max || signed_x < min) {
    return ARMRelocationFactory::Overflow;
  } else {
    return ARMRelocationFactory::OK;
  }
}

// R_ARM_THM_MOVW_ABS_NC: (S + A) | T
// R_ARM_THM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocationFactory::Result thm_movw(Relocation& pReloc,
                                      ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord target = (pReloc.target() << 16);
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(target);
  ARMRelocationFactory::DWord X;
  if (pReloc.type() == R_ARM_THM_MOVW_ABS_NC) {
    X = (S + A) | T;
  } else {
    X = ((S + A) | T) - P;
  }

  target = helper_insert_val_thumb_movw_movt_inst(target, X);

  pReloc.target() &= 0xffff0000U;
  pReloc.target() |= (target >> 16);
  return ARMRelocationFactory::OK;
}

// R_ARM_THM_MOVT_ABS: S + A
// R_ARM_THM_MOVT_PREF: S + A - P
ARMRelocationFactory::Result thm_movt(Relocation& pReloc,
                                      ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord target = (pReloc.target() << 16);
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(target);
  ARMRelocationFactory::DWord X;
  if (pReloc.type() == R_ARM_THM_MOVT_ABS) {
    X = S + A;
  } else {
    X = S + A - P;
  }

  target = helper_insert_val_thumb_movw_movt_inst(target, X);

  pReloc.target() &= 0xffff0000U;
  pReloc.target() |= (target >> 16);
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

