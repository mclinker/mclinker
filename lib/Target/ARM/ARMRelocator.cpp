//===- ARMRelocator.cpp  ----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <llvm/Support/DataTypes.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/Host.h>
#include <mcld/Support/MsgHandling.h>
#include "ARMRelocator.h"
#include "ARMRelocationFunctions.h"

using namespace mcld;

//===--------------------------------------------------------------------===//
// Relocation Functions and Tables
//===--------------------------------------------------------------------===//
DECL_ARM_APPLY_RELOC_FUNCS

/// the prototype of applying function
typedef Relocator::Result (*ApplyFunctionType)(Relocation& pReloc,
                                               ARMRelocator& pParent);

// the table entry of applying functions
struct ApplyFunctionTriple
{
  ApplyFunctionType func;
  unsigned int type;
  const char* name;
};

// declare the table of applying functions
static const ApplyFunctionTriple ApplyFunctions[] = {
  DECL_ARM_APPLY_RELOC_FUNC_PTRS
};

//===--------------------------------------------------------------------===//
// ARMRelocator
//===--------------------------------------------------------------------===//
ARMRelocator::ARMRelocator(ARMGNULDBackend& pParent)
  : Relocator(),
    m_Target(pParent) {
}

ARMRelocator::~ARMRelocator()
{
}

Relocator::Result
ARMRelocator::applyRelocation(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();
  if (type > 130) { // 131-255 doesn't noted in ARM spec
    return Relocator::Unknown;
  }

  return ApplyFunctions[type].func(pRelocation, *this);
}

const char* ARMRelocator::getName(Relocator::Type pType) const
{
  return ApplyFunctions[pType].name;
}

Relocator::Size ARMRelocator::getSize(Relocation::Type pType) const
{
  return 32;
}

//===--------------------------------------------------------------------===//
// non-member functions
//===--------------------------------------------------------------------===//
static Relocator::DWord getThumbBit(const Relocation& pReloc)
{
  // Set thumb bit if
  // - symbol has type of STT_FUNC, is defined and with bit 0 of its value set
  Relocator::DWord thumbBit =
       ((!pReloc.symInfo()->isUndef() || pReloc.symInfo()->isDyn()) &&
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
uint64_t helper_bit_select(uint64_t pA, uint64_t pB, uint64_t pMask)
{
  return (pA & ~pMask) | (pB & pMask) ;
}

// Check if symbol can use relocation R_ARM_RELATIVE
static bool
helper_use_relative_reloc(const ResolveInfo& pSym,
                          const ARMRelocator& pFactory)
{
  // if symbol is dynamic or undefine or preemptible
  if (pSym.isDyn() ||
      pSym.isUndef() ||
      pFactory.getTarget().isSymbolPreemptible(pSym))
    return false;
  return true;
}

static
ARMGOTEntry& helper_get_GOT_and_init(Relocation& pReloc,
                                     ARMRelocator& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  ARMGNULDBackend& ld_backend = pParent.getTarget();

  ARMGOTEntry* got_entry = pParent.getSymGOTMap().lookUp(*rsym);
  if (NULL == got_entry) {
    got_entry = ld_backend.getGOT().consumeGOT();
    pParent.getSymGOTMap().record(*rsym, *got_entry);
    // If we first get this GOT entry, we should initialize it.
    if (rsym->reserved() & ARMGNULDBackend::ReserveGOT) {
      // No corresponding dynamic relocation, initialize to the symbol value.
      got_entry->setValue(pReloc.symValue());
    }
    else if (rsym->reserved() & ARMGNULDBackend::GOTRel) {

      // Initialize corresponding dynamic relocation.
      Relocation& rel_entry = *ld_backend.getRelDyn().consumeEntry();
      if ( rsym->isLocal() ||
          helper_use_relative_reloc(*rsym, pParent)) {
        // Initialize got entry to target symbol address
        got_entry->setValue(pReloc.symValue());
        rel_entry.setType(llvm::ELF::R_ARM_RELATIVE);
        rel_entry.setSymInfo(0);
      }
      else {
        // Initialize got entry to 0 for corresponding dynamic relocation.
        got_entry->setValue(0);
        rel_entry.setType(llvm::ELF::R_ARM_GLOB_DAT);
        rel_entry.setSymInfo(rsym);
      }
      rel_entry.targetRef().assign(*got_entry);
    }
    else {
      fatal(diag::reserve_entry_number_mismatch_got);
    }
  }
  return *got_entry;
}

static
ARMRelocator::Address helper_GOT_ORG(ARMRelocator& pParent)
{
  return pParent.getTarget().getGOT().addr();
}


static
ARMRelocator::Address helper_GOT(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMGOTEntry& got_entry = helper_get_GOT_and_init(pReloc, pParent);
  return helper_GOT_ORG(pParent) + got_entry.getOffset();
}


static
ARMPLT1& helper_get_PLT_and_init(Relocation& pReloc, ARMRelocator& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  ARMGNULDBackend& ld_backend = pParent.getTarget();

  ARMPLT1* plt_entry = pParent.getSymPLTMap().lookUp(*rsym);
  if (NULL != plt_entry)
    return *plt_entry;

  plt_entry = ld_backend.getPLT().consume();
  pParent.getSymPLTMap().record(*rsym, *plt_entry);

  // If we first get this PLT entry, we should initialize it.
  if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
    ARMGOTEntry* gotplt_entry = pParent.getSymGOTPLTMap().lookUp(*rsym);
    assert(NULL == gotplt_entry && "PLT entry not exist, but DynRel entry exist!");
    gotplt_entry = ld_backend.getGOT().consumeGOTPLT();
    pParent.getSymGOTPLTMap().record(*rsym, *gotplt_entry);

    // Initialize corresponding dynamic relocation.
    Relocation& rel_entry = *ld_backend.getRelPLT().consumeEntry();
    rel_entry.setType(llvm::ELF::R_ARM_JUMP_SLOT);
    rel_entry.targetRef().assign(*gotplt_entry);
    rel_entry.setSymInfo(rsym);
  }
  else {
    fatal(diag::reserve_entry_number_mismatch_plt);
  }

  return *plt_entry;
}

static
ARMRelocator::Address helper_PLT_ORG(ARMRelocator& pParent)
{
  return pParent.getTarget().getPLT().addr();
}


static
ARMRelocator::Address helper_PLT(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMPLT1& plt_entry = helper_get_PLT_and_init(pReloc, pParent);
  return helper_PLT_ORG(pParent) + plt_entry.getOffset();
}

// Get an relocation entry in .rel.dyn and set its type to pType,
// its FragmentRef to pReloc->targetFrag() and its ResolveInfo to
// pReloc->symInfo()
static
void helper_DynRel(Relocation& pReloc,
                   ARMRelocator::Type pType,
                   ARMRelocator& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  ARMGNULDBackend& ld_backend = pParent.getTarget();

  Relocation& rel_entry = *ld_backend.getRelDyn().consumeEntry();
  rel_entry.setType(pType);
  rel_entry.targetRef() = pReloc.targetRef();

  if (pType == llvm::ELF::R_ARM_RELATIVE)
    rel_entry.setSymInfo(0);
  else
    rel_entry.setSymInfo(rsym);
}

static ARMRelocator::DWord
helper_extract_movw_movt_addend(ARMRelocator::DWord pTarget)
{
  // imm16: [19-16][11-0]
  return helper_sign_extend((((pTarget >> 4)) & 0xf000U) | (pTarget & 0xfffU),
                            16);
}

static ARMRelocator::DWord
helper_insert_val_movw_movt_inst(ARMRelocator::DWord pTarget,
                                 ARMRelocator::DWord pImm)
{
  // imm16: [19-16][11-0]
  pTarget &= 0xfff0f000U;
  pTarget |= pImm & 0x0fffU;
  pTarget |= (pImm & 0xf000U) << 4;
  return pTarget;
}

static ARMRelocator::DWord
helper_extract_thumb_movw_movt_addend(ARMRelocator::DWord pValue)
{
  // imm16: [19-16][26][14-12][7-0]
  return helper_sign_extend((((pValue >> 4) & 0xf000U) |
                             ((pValue >> 15) & 0x0800U) |
                             ((pValue >> 4) & 0x0700U) |
                             (pValue& 0x00ffU)),
                            16);
}

static ARMRelocator::DWord
helper_insert_val_thumb_movw_movt_inst(ARMRelocator::DWord pValue,
                                       ARMRelocator::DWord pImm)
{
  // imm16: [19-16][26][14-12][7-0]
  pValue &= 0xfbf08f00U;
  pValue |= (pImm & 0xf000U) << 4;
  pValue |= (pImm & 0x0800U) << 15;
  pValue |= (pImm & 0x0700U) << 4;
  pValue |= (pImm & 0x00ffU);
  return pValue;
}

static ARMRelocator::DWord
helper_thumb32_branch_offset(ARMRelocator::DWord pUpper16,
                             ARMRelocator::DWord pLower16)
{
  ARMRelocator::DWord s = (pUpper16 & (1U << 10)) >> 10,        // 26 bit
                           u  = pUpper16 & 0x3ffU,              // 25-16
                           l  = pLower16 & 0x7ffU,              // 10-0
                           j1 = (pLower16 & (1U << 13)) >> 13,  // 13
                           j2 = (pLower16 & (1U << 11)) >> 11;  // 11
  ARMRelocator::DWord i1 = j1 ^ s? 0: 1,
                              i2 = j2 ^ s? 0: 1;

  // [31-25][24][23][22][21-12][11-1][0]
  //      0   s  i1  i2      u     l  0
  return helper_sign_extend((s << 24) | (i1 << 23) | (i2 << 22) |
                            (u << 12) | (l << 1),
                            25);
}

static ARMRelocator::DWord
helper_thumb32_branch_upper(ARMRelocator::DWord pUpper16,
                            ARMRelocator::DWord pOffset)
{
  uint32_t sign = ((pOffset & 0x80000000U) >> 31);
  return (pUpper16 & ~0x7ffU) | ((pOffset >> 12) & 0x3ffU) | (sign << 10);
}

static ARMRelocator::DWord
helper_thumb32_branch_lower(ARMRelocator::DWord pLower16,
                            ARMRelocator::DWord pOffset)
{
  uint32_t sign = ((pOffset & 0x80000000U) >> 31);
  return ((pLower16 & ~0x2fffU) |
          ((((pOffset >> 23) & 1) ^ !sign) << 13) |
          ((((pOffset >> 22) & 1) ^ !sign) << 11) |
          ((pOffset >> 1) & 0x7ffU));
}

// Return true if overflow
static bool
helper_check_signed_overflow(ARMRelocator::DWord pValue,
                             unsigned bits)
{
  int32_t signed_val = static_cast<int32_t>(pValue);
  int32_t max = (1 << (bits - 1)) - 1;
  int32_t min = -(1 << (bits - 1));
  if (signed_val > max || signed_val < min) {
    return true;
  } else {
    return false;
  }
}


//=========================================//
// Each relocation function implementation //
//=========================================//

// R_ARM_NONE
ARMRelocator::Result none(Relocation& pReloc, ARMRelocator& pParent)
{
  return ARMRelocator::OK;
}

// R_ARM_ABS32: (S + A) | T
ARMRelocator::Result abs32(Relocation& pReloc, ARMRelocator& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocator::DWord T = getThumbBit(pReloc);
  ARMRelocator::DWord A = pReloc.target() + pReloc.addend();
  ARMRelocator::DWord S = pReloc.symValue();

  LDSection& target_sect = pReloc.targetRef().frag()->getParent()->getSection();

  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 == (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    pReloc.target() = (S + A) | T;
    return ARMRelocator::OK;
  }

  // A local symbol may need REL Type dynamic relocation
  if (rsym->isLocal() && (rsym->reserved() & ARMGNULDBackend::ReserveRel)) {
    helper_DynRel(pReloc, llvm::ELF::R_ARM_RELATIVE, pParent);
    pReloc.target() = (S + A) | T ;
    return ARMRelocator::OK;
  }

  // An external symbol may need PLT and dynamic relocation
  if (!rsym->isLocal()) {
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      T = 0 ; // PLT is not thumb
    }
    // If we generate a dynamic relocation (except R_ARM_RELATIVE)
    // for a place, we should not perform static relocation on it
    // in order to keep the addend store in the place correct.
    if (rsym->reserved() & ARMGNULDBackend::ReserveRel) {
      if (helper_use_relative_reloc(*rsym, pParent)) {
        helper_DynRel(pReloc, llvm::ELF::R_ARM_RELATIVE, pParent);
      }
      else {
        helper_DynRel(pReloc, pReloc.type(), pParent);
        return ARMRelocator::OK;
      }
    }
  }


  // perform static relocation
  pReloc.target() = (S + A) | T;
  return ARMRelocator::OK;
}

// R_ARM_REL32: ((S + A) | T) - P
ARMRelocator::Result rel32(Relocation& pReloc, ARMRelocator& pParent)
{
  // perform static relocation
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord   T = getThumbBit(pReloc);
  ARMRelocator::DWord   A = pReloc.target() + pReloc.addend();

  // An external symbol may need PLT (this reloc is from stub)
  if (!pReloc.symInfo()->isLocal()) {
    if (pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      T = 0;  // PLT is not thumb.
    }
  }

  // perform relocation
  pReloc.target() = ((S + A) | T) - pReloc.place();

  return ARMRelocator::OK;
}

// R_ARM_BASE_PREL: B(S) + A - P
ARMRelocator::Result base_prel(Relocation& pReloc, ARMRelocator& pParent)
{
  // perform static relocation
  ARMRelocator::DWord A = pReloc.target() + pReloc.addend();
  pReloc.target() = pReloc.symValue() + A - pReloc.place();
  return ARMRelocator::OK;
}

// R_ARM_GOTOFF32: ((S + A) | T) - GOT_ORG
ARMRelocator::Result gotoff32(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMRelocator::DWord T = getThumbBit(pReloc);
  ARMRelocator::DWord A = pReloc.target() + pReloc.addend();
  ARMRelocator::Address GOT_ORG = helper_GOT_ORG(pParent);
  ARMRelocator::Address S = pReloc.symValue();

  pReloc.target() = ((S + A) | T) - GOT_ORG;
  return ARMRelocator::OK;
}

// R_ARM_GOT_BREL: GOT(S) + A - GOT_ORG
ARMRelocator::Result got_brel(Relocation& pReloc, ARMRelocator& pParent)
{
  if (!(pReloc.symInfo()->reserved() &
      (ARMGNULDBackend::ReserveGOT | ARMGNULDBackend::GOTRel))) {
    return ARMRelocator::BadReloc;
  }
  ARMRelocator::Address GOT_S   = helper_GOT(pReloc, pParent);
  ARMRelocator::DWord   A       = pReloc.target() + pReloc.addend();
  ARMRelocator::Address GOT_ORG = helper_GOT_ORG(pParent);
  // Apply relocation.
  pReloc.target() = GOT_S + A - GOT_ORG;
  return ARMRelocator::OK;
}

// R_ARM_GOT_PREL: GOT(S) + A - P
ARMRelocator::Result got_prel(Relocation& pReloc, ARMRelocator& pParent)
{
  if (!(pReloc.symInfo()->reserved() &
      (ARMGNULDBackend::ReserveGOT | ARMGNULDBackend::GOTRel))) {
    return ARMRelocator::BadReloc;
  }
  ARMRelocator::Address GOT_S   = helper_GOT(pReloc, pParent);
  ARMRelocator::DWord   A       = pReloc.target() + pReloc.addend();
  ARMRelocator::Address P = pReloc.place();

  // Apply relocation.
  pReloc.target() = GOT_S + A - P;
  return ARMRelocator::OK;
}

// R_ARM_PLT32: ((S + A) | T) - P
// R_ARM_JUMP24: ((S + A) | T) - P
// R_ARM_CALL: ((S + A) | T) - P
ARMRelocator::Result call(Relocation& pReloc, ARMRelocator& pParent)
{
  // If target is undefined weak symbol, we only need to jump to the
  // next instruction unless it has PLT entry. Rewrite instruction
  // to NOP.
  if (pReloc.symInfo()->isWeak() &&
      pReloc.symInfo()->isUndef() &&
      !pReloc.symInfo()->isDyn() &&
      !(pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT)) {
    // change target to NOP : mov r0, r0
    pReloc.target() = (pReloc.target() & 0xf0000000U) | 0x01a00000;
    return ARMRelocator::OK;
  }

  ARMRelocator::Address S; // S depends on PLT exists or not.
  ARMRelocator::DWord   T = getThumbBit(pReloc);
  ARMRelocator::DWord   A =
    helper_sign_extend((pReloc.target() & 0x00FFFFFFu) << 2, 26)
    + pReloc.addend();
  ARMRelocator::Address P = pReloc.place();

  S = pReloc.symValue();
  if (pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT) {
    S = helper_PLT(pReloc, pParent);
    T = 0;  // PLT is not thumb.
  }

  // At this moment (after relaxation), if the jump target is thumb instruction,
  // switch mode is needed, rewrite the instruction to BLX
  // FIXME: check if we can use BLX instruction (check from .ARM.attribute
  // CPU ARCH TAG, which should be ARMv5 or above)
  if (T != 0) {
    // cannot rewrite to blx for R_ARM_JUMP24
    if (pReloc.type() == llvm::ELF::R_ARM_JUMP24)
      return ARMRelocator::BadReloc;

    pReloc.target() = (pReloc.target() & 0xffffff) |
                      0xfa000000 |
                      (((S + A - P) & 2) << 23);
  }

  ARMRelocator::DWord X = ((S + A) | T) - P;
  // Check X is 24bit sign int. If not, we should use stub or PLT before apply.
  if (helper_check_signed_overflow(X, 26))
    return ARMRelocator::Overflow;
  //                    Make sure the Imm is 0.          Result Mask.
  pReloc.target() = (pReloc.target() & 0xFF000000u) | ((X & 0x03FFFFFEu) >> 2);
  return ARMRelocator::OK;
}

// R_ARM_THM_CALL: ((S + A) | T) - P
// R_ARM_THM_JUMP24: (((S + A) | T) - P)
ARMRelocator::Result thm_call(Relocation& pReloc, ARMRelocator& pParent)
{
  // If target is undefined weak symbol, we only need to jump to the
  // next instruction unless it has PLT entry. Rewrite instruction
  // to NOP.
  if (pReloc.symInfo()->isWeak() &&
      pReloc.symInfo()->isUndef() &&
      !pReloc.symInfo()->isDyn() &&
      !(pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT)) {
    pReloc.target() = (0xe000U << 16) | 0xbf00U;
    return ARMRelocator::OK;
  }

  // get lower and upper 16 bit instructions from relocation targetData
  uint16_t upper_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()));
  uint16_t lower_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1);

  ARMRelocator::DWord T = getThumbBit(pReloc);
  ARMRelocator::DWord A = helper_thumb32_branch_offset(upper_inst,
                                                               lower_inst);
  ARMRelocator::Address P = pReloc.place();
  ARMRelocator::Address S;

  // if symbol has plt
  if (pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT) {
    S = helper_PLT(pReloc, pParent);
    T = 0;  // PLT is not thumb.
  }
  else {
    S = pReloc.symValue();
  }

  S = S + A;

  // At this moment (after relaxation), if the jump target is arm
  // instruction, switch mode is needed, rewrite the instruction to BLX
  // FIXME: check if we can use BLX instruction (check from .ARM.attribute
  // CPU ARCH TAG, which should be ARMv5 or above)
  if (T == 0) {
    // cannot rewrite to blx for R_ARM_THM_JUMP24
    if (pReloc.type() == llvm::ELF::R_ARM_THM_JUMP24)
      return ARMRelocator::BadReloc;

    // for BLX, select bit 1 from relocation base address to jump target
    // address
    S = helper_bit_select(S, P, 0x2);
    // rewrite instruction to BLX
    lower_inst &= ~0x1000U;
  }
  else {
    // otherwise, the instruction should be BL
    lower_inst |= 0x1000U;
  }

  ARMRelocator::DWord X = (S | T) - P;

  // FIXME: Check bit size is 24(thumb2) or 22?
  if (helper_check_signed_overflow(X, 25)) {
    return ARMRelocator::Overflow;
  }

  upper_inst = helper_thumb32_branch_upper(upper_inst, X);
  lower_inst = helper_thumb32_branch_lower(lower_inst, X);

  *(reinterpret_cast<uint16_t*>(&pReloc.target())) = upper_inst;
  *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1) = lower_inst;

  return ARMRelocator::OK;
}

// R_ARM_MOVW_ABS_NC: (S + A) | T
ARMRelocator::Result movw_abs_nc(Relocation& pReloc, ARMRelocator& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord T = getThumbBit(pReloc);
  ARMRelocator::DWord A =
      helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocator::DWord X;

  LDSection& target_sect = pReloc.targetRef().frag()->getParent()->getSection();

  // If the flag of target section is not ALLOC, we will not scan this
  // relocation but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    // use plt
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      T = 0 ; // PLT is not thumb
    }
  }

  // perform static relocation
  X = (S + A) | T;
  pReloc.target() = helper_insert_val_movw_movt_inst(
                                         pReloc.target() + pReloc.addend(), X);
  return ARMRelocator::OK;
}

// R_ARM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocator::Result movw_prel_nc(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord T = getThumbBit(pReloc);
  ARMRelocator::DWord P = pReloc.place();
  ARMRelocator::DWord A =
      helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocator::DWord X;

  X = ((S + A) | T) - P;

  if (helper_check_signed_overflow(X, 16)) {
    return ARMRelocator::Overflow;
  } else {
    pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
    return ARMRelocator::OK;
  }
}

// R_ARM_MOVT_ABS: S + A
ARMRelocator::Result movt_abs(Relocation& pReloc, ARMRelocator& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord A =
    helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocator::DWord X;

  LDSection& target_sect = pReloc.targetRef().frag()->getParent()->getSection();

  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    // use plt
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
    }
  }

  X = S + A;
  X >>= 16;
  // perform static relocation
  pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
  return ARMRelocator::OK;
}

// R_ARM_MOVT_PREL: S + A - P
ARMRelocator::Result movt_prel(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord P = pReloc.place();
  ARMRelocator::DWord A =
            helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocator::DWord X;

  X = S + A - P;
  X >>= 16;

  pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
  return ARMRelocator::OK;
}

// R_ARM_THM_MOVW_ABS_NC: (S + A) | T
ARMRelocator::Result thm_movw_abs_nc(Relocation& pReloc, ARMRelocator& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord T = getThumbBit(pReloc);

  // get lower and upper 16 bit instructions from relocation targetData
  uint16_t upper_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()));
  uint16_t lower_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1);
  ARMRelocator::DWord val = ((upper_inst) << 16) | (lower_inst);
  ARMRelocator::DWord A =
      helper_extract_thumb_movw_movt_addend(val) + pReloc.addend();
  ARMRelocator::DWord X;

  LDSection& target_sect = pReloc.targetRef().frag()->getParent()->getSection();
  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    // use plt
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      T = 0; // PLT is not thumb
    }
  }
  X = (S + A) | T;

  val = helper_insert_val_thumb_movw_movt_inst(val, X);
  *(reinterpret_cast<uint16_t*>(&pReloc.target())) = upper_inst;
  *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1) = lower_inst;

  return ARMRelocator::OK;
}

// R_ARM_THM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocator::Result thm_movw_prel_nc(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord T = getThumbBit(pReloc);
  ARMRelocator::DWord P = pReloc.place();

  // get lower and upper 16 bit instructions from relocation targetData
  uint16_t upper_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()));
  uint16_t lower_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1);
  ARMRelocator::DWord val = ((upper_inst) << 16) | (lower_inst);
  ARMRelocator::DWord A =
      helper_extract_thumb_movw_movt_addend(val) + pReloc.addend();
  ARMRelocator::DWord X;

  X = ((S + A) | T) - P;

  val = helper_insert_val_thumb_movw_movt_inst(val, X);
  *(reinterpret_cast<uint16_t*>(&pReloc.target())) = upper_inst;
  *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1) = lower_inst;

  return ARMRelocator::OK;
}

// R_ARM_THM_MOVW_BREL_NC: ((S + A) | T) - B(S)
// R_ARM_THM_MOVW_BREL: ((S + A) | T) - B(S)
ARMRelocator::Result thm_movw_brel(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord T = getThumbBit(pReloc);
  ARMRelocator::DWord P = pReloc.place();

  // get lower and upper 16 bit instructions from relocation targetData
  uint16_t upper_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()));
  uint16_t lower_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1);
  ARMRelocator::DWord val = ((upper_inst) << 16) | (lower_inst);
  ARMRelocator::DWord A =
      helper_extract_thumb_movw_movt_addend(val) + pReloc.addend();
  ARMRelocator::DWord X;

  X = ((S + A) | T) - P;

  val = helper_insert_val_thumb_movw_movt_inst(val, X);
  *(reinterpret_cast<uint16_t*>(&pReloc.target())) = upper_inst;
  *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1) = lower_inst;

  return ARMRelocator::OK;
}

// R_ARM_THM_MOVT_ABS: S + A
ARMRelocator::Result thm_movt_abs(Relocation& pReloc, ARMRelocator& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocator::Address S = pReloc.symValue();

  // get lower and upper 16 bit instructions from relocation targetData
  uint16_t upper_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()));
  uint16_t lower_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1);
  ARMRelocator::DWord val = ((upper_inst) << 16) | (lower_inst);
  ARMRelocator::DWord A =
      helper_extract_thumb_movw_movt_addend(val) + pReloc.addend();
  ARMRelocator::DWord X;

  LDSection& target_sect = pReloc.targetRef().frag()->getParent()->getSection();
  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    // use plt
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
    }
  }

  X = S + A;
  X >>= 16;

  // check 16-bit overflow
  if (helper_check_signed_overflow(X, 16)) {
    return ARMRelocator::Overflow;
  } else {
    val = helper_insert_val_thumb_movw_movt_inst(val, X);
    *(reinterpret_cast<uint16_t*>(&pReloc.target())) = upper_inst;
    *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1) = lower_inst;
    return ARMRelocator::OK;
  }
}

// R_ARM_THM_MOVT_PREL: S + A - P
// R_ARM_THM_MOVT_BREL: S + A - B(S)
ARMRelocator::Result thm_movt_prel(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMRelocator::Address S = pReloc.symValue();
  ARMRelocator::DWord P = pReloc.place();

  // get lower and upper 16 bit instructions from relocation targetData
  uint16_t upper_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()));
  uint16_t lower_inst = *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1);
  ARMRelocator::DWord val = ((upper_inst) << 16) | (lower_inst);
  ARMRelocator::DWord A =
      helper_extract_thumb_movw_movt_addend(val) + pReloc.addend();
  ARMRelocator::DWord X;

  X = S + A - P;
  X >>= 16;

  val = helper_insert_val_thumb_movw_movt_inst(val, X);
  *(reinterpret_cast<uint16_t*>(&pReloc.target())) = upper_inst;
  *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1) = lower_inst;

  return ARMRelocator::OK;
}

// R_ARM_PREL31: ((S + A) | T) - P
ARMRelocator::Result prel31(Relocation& pReloc, ARMRelocator& pParent)
{
  ARMRelocator::DWord target = pReloc.target();
  ARMRelocator::DWord T = getThumbBit(pReloc);
  ARMRelocator::DWord A = helper_sign_extend(target, 31) +
                                  pReloc.addend();
  ARMRelocator::DWord P = pReloc.place();
  ARMRelocator::Address S;

  S = pReloc.symValue();
  // if symbol has plt
  if ( pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT) {
    S = helper_PLT(pReloc, pParent);
    T = 0;  // PLT is not thumb.
  }

  ARMRelocator::DWord X = ((S + A) | T) - P;
  pReloc.target() = helper_bit_select(target, X, 0x7fffffffU);
  if (helper_check_signed_overflow(X, 31))
    return ARMRelocator::Overflow;
  return ARMRelocator::OK;
}

// R_ARM_TLS_GD32: GOT(S) + A - P
// R_ARM_TLS_IE32: GOT(S) + A - P
// R_ARM_TLS_LE32: S + A - tp
ARMRelocator::Result tls(Relocation& pReloc, ARMRelocator& pParent)
{
  return ARMRelocator::Unsupport;
}

ARMRelocator::Result unsupport(Relocation& pReloc, ARMRelocator& pParent)
{
  return ARMRelocator::Unsupport;
}
