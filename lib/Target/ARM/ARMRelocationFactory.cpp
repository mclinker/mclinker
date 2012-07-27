//===- ARMRelocationFactory.cpp  ----------------------------------------===//
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
#include <mcld/MC/MCLDInfo.h>
#include <mcld/LD/Layout.h>
#include <mcld/Support/MsgHandling.h>

#include "ARMRelocationFactory.h"
#include "ARMRelocationFunctions.h"

using namespace mcld;

//===--------------------------------------------------------------------===//
// Relocation Functions and Tables
//===--------------------------------------------------------------------===//
DECL_ARM_APPLY_RELOC_FUNCS

/// the prototype of applying function
typedef RelocationFactory::Result (*ApplyFunctionType)(
                                               Relocation& pReloc,
                                               const MCLDInfo& pLDInfo,
                                               ARMRelocationFactory& pParent);

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
// ARMRelocationFactory
//===--------------------------------------------------------------------===//
ARMRelocationFactory::ARMRelocationFactory(size_t pNum,
                                           ARMGNULDBackend& pParent)
  : RelocationFactory(pNum),
    m_Target(pParent) {
}

ARMRelocationFactory::~ARMRelocationFactory()
{
}

RelocationFactory::Result
ARMRelocationFactory::applyRelocation(Relocation& pRelocation,
                                      const MCLDInfo& pLDInfo)
{
  Relocation::Type type = pRelocation.type();
  if (type > 130) { // 131-255 doesn't noted in ARM spec
    fatal(diag::unknown_relocation) << (int)type
                                    << pRelocation.symInfo()->name();
    return RelocationFactory::Unknown;
  }

  return ApplyFunctions[type].func(pRelocation, pLDInfo, *this);
}

const char* ARMRelocationFactory::getName(RelocationFactory::Type pType) const
{
  return ApplyFunctions[pType].name;
}

//===--------------------------------------------------------------------===//
// non-member functions
static RelocationFactory::DWord getThumbBit(const Relocation& pReloc)
{
  // Set thumb bit if
  // - symbol has type of STT_FUNC, is defined and with bit 0 of its value set
  RelocationFactory::DWord thumbBit =
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
                          const MCLDInfo& pLDInfo,
                          const ARMRelocationFactory& pFactory)
{
  // if symbol is dynamic or undefine or preemptible
  if (pSym.isDyn() ||
      pSym.isUndef() ||
      pFactory.getTarget().isSymbolPreemptible(pSym,
                                               pLDInfo,
                                               pLDInfo.output()))
    return false;
  return true;
}

static
GOTEntry& helper_get_GOT_and_init(Relocation& pReloc,
                                  const MCLDInfo& pLDInfo,
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

      // Initialize corresponding dynamic relocation.
      Relocation& rel_entry =
        *ld_backend.getRelDyn().getEntry(*rsym, true, exist);
      assert(!exist && "GOT entry not exist, but DynRel entry exist!");
      if ( rsym->isLocal() ||
          helper_use_relative_reloc(*rsym, pLDInfo, pParent)) {
        // Initialize got entry to target symbol address
        got_entry.setContent(pReloc.symValue());
        rel_entry.setType(llvm::ELF::R_ARM_RELATIVE);
        rel_entry.setSymInfo(0);
      }
      else {
        // Initialize got entry to 0 for corresponding dynamic relocation.
        got_entry.setContent(0);
        rel_entry.setType(llvm::ELF::R_ARM_GLOB_DAT);
        rel_entry.setSymInfo(rsym);
      }
      rel_entry.targetRef().assign(got_entry);
    }
    else {
      fatal(diag::reserve_entry_number_mismatch_got);
    }
  }
  return got_entry;
}

static
ARMRelocationFactory::Address helper_GOT_ORG(ARMRelocationFactory& pParent)
{
  return pParent.getTarget().getGOT().getSection().addr();
}


static
ARMRelocationFactory::Address helper_GOT(Relocation& pReloc,
                                         const MCLDInfo& pLDInfo,
                                         ARMRelocationFactory& pParent)
{
  GOTEntry& got_entry = helper_get_GOT_and_init(pReloc, pLDInfo, pParent);
  return helper_GOT_ORG(pParent) + pParent.getLayout().getOutputOffset(got_entry);
}


static
PLTEntry& helper_get_PLT_and_init(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  ARMGNULDBackend& ld_backend = pParent.getTarget();

  bool exist;
  PLTEntry& plt_entry = *ld_backend.getPLT().getPLTEntry(*rsym, exist);
  if (!exist) {
    // If we first get this PLT entry, we should initialize it.
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      GOTEntry& gotplt_entry =
        *ld_backend.getPLT().getGOTPLTEntry(*rsym, exist);
      // Initialize corresponding dynamic relocation.
      Relocation& rel_entry =
        *ld_backend.getRelPLT().getEntry(*rsym, true, exist);
      assert(!exist && "PLT entry not exist, but DynRel entry exist!");
      rel_entry.setType(llvm::ELF::R_ARM_JUMP_SLOT);
      rel_entry.targetRef().assign(gotplt_entry);
      rel_entry.setSymInfo(rsym);
    }
    else {
      fatal(diag::reserve_entry_number_mismatch_plt);
    }
  }
  return plt_entry;
}



static
ARMRelocationFactory::Address helper_PLT_ORG(ARMRelocationFactory& pParent)
{
  return pParent.getTarget().getPLT().getSection().addr();
}


static
ARMRelocationFactory::Address helper_PLT(Relocation& pReloc,
                                         ARMRelocationFactory& pParent)
{
  PLTEntry& plt_entry = helper_get_PLT_and_init(pReloc, pParent);
  return helper_PLT_ORG(pParent) + pParent.getLayout().getOutputOffset(plt_entry);
}

// Get an relocation entry in .rel.dyn and set its type to pType,
// its FragmentRef to pReloc->targetFrag() and its ResolveInfo to pReloc->symInfo()
static
void helper_DynRel(Relocation& pReloc,
                   ARMRelocationFactory::Type pType,
                   ARMRelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  ARMGNULDBackend& ld_backend = pParent.getTarget();
  bool exist;

  Relocation& rel_entry =
    *ld_backend.getRelDyn().getEntry(*rsym, false, exist);
  rel_entry.setType(pType);
  rel_entry.targetRef() = pReloc.targetRef();

  if (pType == llvm::ELF::R_ARM_RELATIVE)
    rel_entry.setSymInfo(0);
  else
    rel_entry.setSymInfo(rsym);
}

static ARMRelocationFactory::DWord
helper_extract_movw_movt_addend(ARMRelocationFactory::DWord pTarget)
{
  // imm16: [19-16][11-0]
  return helper_sign_extend((((pTarget >> 4)) & 0xf000U) | (pTarget & 0xfffU),
                            16);
}

static ARMRelocationFactory::DWord
helper_insert_val_movw_movt_inst(ARMRelocationFactory::DWord pTarget,
                                 ARMRelocationFactory::DWord pImm)
{
  // imm16: [19-16][11-0]
  pTarget &= 0xfff0f000U;
  pTarget |= pImm & 0x0fffU;
  pTarget |= (pImm & 0xf000U) << 4;
  return pTarget;
}

static ARMRelocationFactory::DWord
helper_extract_thumb_movw_movt_addend(ARMRelocationFactory::DWord pTarget)
{
  // Consider the endianness problem, get the target data value from lower
  // and upper 16 bits
  ARMRelocationFactory::DWord val =
    (*(reinterpret_cast<uint16_t*>(&pTarget)) << 16) |
    *(reinterpret_cast<uint16_t*>(&pTarget) + 1);

  // imm16: [19-16][26][14-12][7-0]
  return helper_sign_extend((((val >> 4) & 0xf000U) |
                             ((val >> 15) & 0x0800U) |
                             ((val >> 4) & 0x0700U) |
                             (val & 0x00ffU)),
                            16);
}

static ARMRelocationFactory::DWord
helper_insert_val_thumb_movw_movt_inst(ARMRelocationFactory::DWord pTarget,
                                       ARMRelocationFactory::DWord pImm)
{
  ARMRelocationFactory::DWord val;
  // imm16: [19-16][26][14-12][7-0]
  pTarget &= 0xfbf08f00U;
  pTarget |= (pImm & 0xf000U) << 4;
  pTarget |= (pImm & 0x0800U) << 15;
  pTarget |= (pImm & 0x0700U) << 4;
  pTarget |= (pImm & 0x00ffU);

  // Consider the endianness problem, write back data from lower and
  // upper 16 bits
  val = (*(reinterpret_cast<uint16_t*>(&pTarget)) << 16) |
        *(reinterpret_cast<uint16_t*>(&pTarget) + 1);
  return val;
}

static ARMRelocationFactory::DWord
helper_thumb32_branch_offset(ARMRelocationFactory::DWord pUpper16,
                             ARMRelocationFactory::DWord pLower16)
{
  ARMRelocationFactory::DWord s = (pUpper16 & (1U << 10)) >> 10,  // 26 bit
                              u = pUpper16 & 0x3ffU,              // 25-16
                              l = pLower16 & 0x7ffU,              // 10-0
                             j1 = (pLower16 & (1U << 13)) >> 13,  // 13
                             j2 = (pLower16 & (1U << 11)) >> 11;  // 11
  ARMRelocationFactory::DWord i1 = j1 ^ s? 0: 1,
                              i2 = j2 ^ s? 0: 1;

  // [31-25][24][23][22][21-12][11-1][0]
  //      0   s  i1  i2      u     l  0
  return helper_sign_extend((s << 24) | (i1 << 23) | (i2 << 22) |
                            (u << 12) | (l << 1),
                            25);
}

static ARMRelocationFactory::DWord
helper_thumb32_branch_upper(ARMRelocationFactory::DWord pUpper16,
                            ARMRelocationFactory::DWord pOffset)
{
  uint32_t sign = ((pOffset & 0x80000000U) >> 31);
  return (pUpper16 & ~0x7ffU) | ((pOffset >> 12) & 0x3ffU) | (sign << 10);
}

static ARMRelocationFactory::DWord
helper_thumb32_branch_lower(ARMRelocationFactory::DWord pLower16,
                            ARMRelocationFactory::DWord pOffset)
{
  uint32_t sign = ((pOffset & 0x80000000U) >> 31);
  return ((pLower16 & ~0x2fffU) |
          ((((pOffset >> 23) & 1) ^ !sign) << 13) |
          ((((pOffset >> 22) & 1) ^ !sign) << 11) |
          ((pOffset >> 1) & 0x7ffU));
}

// Return true if overflow
static bool
helper_check_signed_overflow(ARMRelocationFactory::DWord pValue,
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
ARMRelocationFactory::Result none(Relocation& pReloc,
                                  const MCLDInfo& pLDInfo,
                                  ARMRelocationFactory& pParent)
{
  return ARMRelocationFactory::OK;
}

// R_ARM_ABS32: (S + A) | T
ARMRelocationFactory::Result abs32(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  ARMRelocationFactory::DWord S = pReloc.symValue();

  const LDSection* target_sect = pParent.getLayout().getOutputLDSection(
                                                  *(pReloc.targetRef().frag()));
  assert(NULL != target_sect);

  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 == (llvm::ELF::SHF_ALLOC & target_sect->flag())) {
    pReloc.target() = (S + A) | T;
    return ARMRelocationFactory::OK;
  }

  // A local symbol may need REL Type dynamic relocation
  if (rsym->isLocal() && (rsym->reserved() & ARMGNULDBackend::ReserveRel)) {
    helper_DynRel(pReloc, llvm::ELF::R_ARM_RELATIVE, pParent);
    pReloc.target() = (S + A) | T ;
    return ARMRelocationFactory::OK;
  }

  // An external symbol may need PLT and dynamic relocation
  if (!rsym->isLocal()) {
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      T = 0 ; // PLT is not thumb
      pReloc.target() = (S + A) | T;
    }
    // If we generate a dynamic relocation (except R_ARM_RELATIVE)
    // for a place, we should not perform static relocation on it
    // in order to keep the addend store in the place correct.
    if (rsym->reserved() & ARMGNULDBackend::ReserveRel) {
      if (helper_use_relative_reloc(*rsym, pLDInfo, pParent)) {
        helper_DynRel(pReloc, llvm::ELF::R_ARM_RELATIVE, pParent);
      }
      else {
        helper_DynRel(pReloc, pReloc.type(), pParent);
        return ARMRelocationFactory::OK;
      }
    }
  }


  // perform static relocation
  pReloc.target() = (S + A) | T;
  return ARMRelocationFactory::OK;
}

// R_ARM_REL32: ((S + A) | T) - P
ARMRelocationFactory::Result rel32(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   ARMRelocationFactory& pParent)
{
  // perform static relocation
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  pReloc.target() = ((pReloc.symValue() + A) | T)
      - pReloc.place(pParent.getLayout());
  return ARMRelocationFactory::OK;
}

// R_ARM_BASE_PREL: B(S) + A - P
ARMRelocationFactory::Result base_prel(Relocation& pReloc,
                                       const MCLDInfo& pLDInfo,
                                       ARMRelocationFactory& pParent)
{
  // perform static relocation
  ARMRelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  pReloc.target() = pReloc.symValue() + A - pReloc.place(pParent.getLayout());
  return ARMRelocationFactory::OK;
}

// R_ARM_GOTOFF32: ((S + A) | T) - GOT_ORG
ARMRelocationFactory::Result gotoff32(Relocation& pReloc,
                                      const MCLDInfo& pLDInfo,
                                      ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  ARMRelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
  ARMRelocationFactory::Address S = pReloc.symValue();

  pReloc.target() = ((S + A) | T) - GOT_ORG;
  return ARMRelocationFactory::OK;
}

// R_ARM_GOT_BREL: GOT(S) + A - GOT_ORG
ARMRelocationFactory::Result got_brel(Relocation& pReloc,
                                      const MCLDInfo& pLDInfo,
                                      ARMRelocationFactory& pParent)
{
  if (!(pReloc.symInfo()->reserved() &
      (ARMGNULDBackend::ReserveGOT | ARMGNULDBackend::GOTRel))) {
    return ARMRelocationFactory::BadReloc;
  }
  ARMRelocationFactory::Address GOT_S   = helper_GOT(pReloc, pLDInfo, pParent);
  ARMRelocationFactory::DWord   A       = pReloc.target() + pReloc.addend();
  ARMRelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
  // Apply relocation.
  pReloc.target() = GOT_S + A - GOT_ORG;
  return ARMRelocationFactory::OK;
}

// R_ARM_GOT_PREL: GOT(S) + A - P
ARMRelocationFactory::Result got_prel(Relocation& pReloc,
                                      const MCLDInfo& pLDInfo,
                                      ARMRelocationFactory& pParent)
{
  if (!(pReloc.symInfo()->reserved() &
      (ARMGNULDBackend::ReserveGOT | ARMGNULDBackend::GOTRel))) {
    return ARMRelocationFactory::BadReloc;
  }
  ARMRelocationFactory::Address GOT_S   = helper_GOT(pReloc, pLDInfo, pParent);
  ARMRelocationFactory::DWord   A       = pReloc.target() + pReloc.addend();
  ARMRelocationFactory::Address P = pReloc.place(pParent.getLayout());
  // Apply relocation.
  pReloc.target() = GOT_S + A - P;
  return ARMRelocationFactory::OK;
}

// R_ARM_PLT32: ((S + A) | T) - P
// R_ARM_JUMP24: ((S + A) | T) - P
// R_ARM_CALL: ((S + A) | T) - P
ARMRelocationFactory::Result call(Relocation& pReloc,
                                  const MCLDInfo& pLDInfo,
                                  ARMRelocationFactory& pParent)
{
  // TODO: Some issue have not been considered:
  // 1. Add stub when switching mode or jump target too far
  // 2. We assume the blx is available

  // If target is undefined weak symbol, we only need to jump to the
  // next instruction unless it has PLT entry. Rewrite instruction
  // to NOP.
  if (pReloc.symInfo()->isWeak() &&
      pReloc.symInfo()->isUndef() &&
      !pReloc.symInfo()->isDyn() &&
      !(pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT)) {
    // change target to NOP : mov r0, r0
    pReloc.target() = (pReloc.target() & 0xf0000000U) | 0x01a00000;
    return ARMRelocationFactory::OK;
  }

  ARMRelocationFactory::Address S; // S depends on PLT exists or not.
  ARMRelocationFactory::DWord   T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord   A =
    helper_sign_extend((pReloc.target() & 0x00FFFFFFu) << 2, 26)
    + pReloc.addend();
  ARMRelocationFactory::Address P = pReloc.place(pParent.getLayout());

  S = pReloc.symValue();
  if (pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT) {
    S = helper_PLT(pReloc, pParent);
    T = 0;  // PLT is not thumb.
  }

  // If the jump target is thumb instruction, switch mode is needed, rewrite
  // the instruction to BLX
  if (T != 0) {
    // cannot rewrite R_ARM_JUMP24 instruction to blx
    assert((pReloc.type() != llvm::ELF::R_ARM_JUMP24)&&
      "Invalid instruction to rewrite to blx for switching mode.");
    pReloc.target() = (pReloc.target() & 0xffffff) |
                      0xfa000000 |
                      (((S + A - P) & 2) << 23);
  }

  ARMRelocationFactory::DWord X = ((S + A) | T) - P;
  // Check X is 24bit sign int. If not, we should use stub or PLT before apply.
  if (helper_check_signed_overflow(X, 26))
    return ARMRelocationFactory::Overflow;
  //                    Make sure the Imm is 0.          Result Mask.
  pReloc.target() = (pReloc.target() & 0xFF000000u) | ((X & 0x03FFFFFEu) >> 2);
  return ARMRelocationFactory::OK;
}

// R_ARM_THM_CALL: ((S + A) | T) - P
// R_ARM_THM_JUMP24: (((S + A) | T) - P)
ARMRelocationFactory::Result thm_call(Relocation& pReloc,
                                      const MCLDInfo& pLDInfo,
                                      ARMRelocationFactory& pParent)
{
  // If target is undefined weak symbol, we only need to jump to the
  // next instruction unless it has PLT entry. Rewrite instruction
  // to NOP.
  if (pReloc.symInfo()->isWeak() &&
      pReloc.symInfo()->isUndef() &&
      !pReloc.symInfo()->isDyn() &&
      !(pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT)) {
    pReloc.target() = (0xe000U << 16) | 0xbf00U;
    return ARMRelocationFactory::OK;
  }

  // get lower and upper 16 bit instructions from relocation targetData
  uint16_t upper16 = *(reinterpret_cast<uint16_t*>(&pReloc.target()));
  uint16_t lower16 = *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1);

  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A = helper_thumb32_branch_offset(upper16,
                                                               lower16);
  ARMRelocationFactory::Address P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::Address S;

  // if symbol has plt
  if (pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT) {
    S = helper_PLT(pReloc, pParent);
    T = 0;  // PLT is not thumb.
  }
  else {
    S = pReloc.symValue();
  }

  S = S + A;

  // FIXME: check if we can use BLX instruction (check from .ARM.attribute
  // CPU ARCH TAG, which should be ARMv5 or above)

  // If the jump target is not thumb, switch mode is needed, rewrite
  // instruction to BLX
  if (T == 0) {
    // for BLX, select bit 1 from relocation base address to jump target
    // address
    S = helper_bit_select(S, P, 0x2);
    // rewrite instruction to BLX
    lower16 &= ~0x1000U;
  }
  else {
    // otherwise, the instruction should be BL
    lower16 |= 0x1000U;
  }

  ARMRelocationFactory::DWord X = (S | T) - P;

  // TODO: check if we need stub when building non-shared object,
  // overflow or switch-mode.

  // FIXME: Check bit size is 24(thumb2) or 22?
  if (helper_check_signed_overflow(X, 25)) {
    return ARMRelocationFactory::Overflow;
  }

  upper16 = helper_thumb32_branch_upper(upper16, X);
  lower16 = helper_thumb32_branch_lower(lower16, X);

 *(reinterpret_cast<uint16_t*>(&pReloc.target())) = upper16;
 *(reinterpret_cast<uint16_t*>(&pReloc.target()) + 1) = lower16;

 return ARMRelocationFactory::OK;
}

// R_ARM_MOVW_ABS_NC: (S + A) | T
ARMRelocationFactory::Result movw_abs_nc(Relocation& pReloc,
                                         const MCLDInfo& pLDInfo,
                                         ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A =
      helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  const LDSection* target_sect = pParent.getLayout().getOutputLDSection(
                                                 *(pReloc.targetRef().frag()));
  assert(NULL != target_sect);
  // If the flag of target section is not ALLOC, we will not scan this
  // relocation but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect->flag())) {
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
  return ARMRelocationFactory::OK;
}

// R_ARM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocationFactory::Result movw_prel_nc(Relocation& pReloc,
                                          const MCLDInfo& pLDInfo,
                                          ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  X = ((S + A) | T) - P;

  if (helper_check_signed_overflow(X, 16)) {
    return ARMRelocationFactory::Overflow;
  } else {
    pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
    return ARMRelocationFactory::OK;
  }
}

// R_ARM_MOVT_ABS: S + A
ARMRelocationFactory::Result movt_abs(Relocation& pReloc,
                                      const MCLDInfo& pLDInfo,
                                      ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord A =
    helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  const LDSection* target_sect = pParent.getLayout().getOutputLDSection(
                                                  *(pReloc.targetRef().frag()));
  assert(NULL != target_sect);
  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect->flag())) {
    // use plt
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
    }
  }

  X = S + A;
  X >>= 16;
  // perform static relocation
  pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
  return ARMRelocationFactory::OK;
}

// R_ARM_MOVT_PREL: S + A - P
ARMRelocationFactory::Result movt_prel(Relocation& pReloc,
                                       const MCLDInfo& pLDInfo,
                                       ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  X = S + A - P;
  X >>= 16;

  pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
  return ARMRelocationFactory::OK;
}

// R_ARM_THM_MOVW_ABS_NC: (S + A) | T
ARMRelocationFactory::Result thm_movw_abs_nc(Relocation& pReloc,
                                             const MCLDInfo& pLDInfo,
                                             ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  const LDSection* target_sect = pParent.getLayout().getOutputLDSection(
                                                  *(pReloc.targetRef().frag()));
  assert(NULL != target_sect);
  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect->flag())) {
    // use plt
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      T = 0; // PLT is not thumb
    }
  }
  X = (S + A) | T;
  pReloc.target() = helper_insert_val_thumb_movw_movt_inst(pReloc.target(),
                                                           X);
  return ARMRelocationFactory::OK;
}

// R_ARM_THM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocationFactory::Result thm_movw_prel_nc(Relocation& pReloc,
                                              const MCLDInfo& pLDInfo,
                                              ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  X = ((S + A) | T) - P;

  // check 16-bit overflow
  pReloc.target() = helper_insert_val_thumb_movw_movt_inst(pReloc.target(),
                                                           X);
  return ARMRelocationFactory::OK;
}

// R_ARM_THM_MOVW_BREL_NC: ((S + A) | T) - B(S)
// R_ARM_THM_MOVW_BREL: ((S + A) | T) - B(S)
ARMRelocationFactory::Result thm_movw_brel(Relocation& pReloc,
                                              const MCLDInfo& pLDInfo,
                                              ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  X = ((S + A) | T) - P;

  // check 16-bit overflow
  pReloc.target() = helper_insert_val_thumb_movw_movt_inst(pReloc.target(),
                                                           X);
  return ARMRelocationFactory::OK;
}

// R_ARM_THM_MOVT_ABS: S + A
ARMRelocationFactory::Result thm_movt_abs(Relocation& pReloc,
                                          const MCLDInfo& pLDInfo,
                                          ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  const LDSection* target_sect = pParent.getLayout().getOutputLDSection(
                                                  *(pReloc.targetRef().frag()));
  assert(NULL != target_sect);
  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 != (llvm::ELF::SHF_ALLOC & target_sect->flag())) {
    // use plt
    if (rsym->reserved() & ARMGNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
    }
  }

  X = S + A;
  X >>= 16;

  // check 16-bit overflow
  if (helper_check_signed_overflow(X, 16)) {
    return ARMRelocationFactory::Overflow;
  } else {
    pReloc.target() = helper_insert_val_thumb_movw_movt_inst(pReloc.target(),
                                                             X);
    return ARMRelocationFactory::OK;
  }
}

// R_ARM_THM_MOVT_PREL: S + A - P
// R_ARM_THM_MOVT_BREL: S + A - B(S)
ARMRelocationFactory::Result thm_movt_prel(Relocation& pReloc,
                                           const MCLDInfo& pLDInfo,
                                           ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  X = S + A - P;
  X >>= 16;
  pReloc.target() = helper_insert_val_thumb_movw_movt_inst(pReloc.target(),
                                                             X);
  return ARMRelocationFactory::OK;
}

// R_ARM_PREL31: (S + A) | T
ARMRelocationFactory::Result prel31(Relocation& pReloc,
                                    const MCLDInfo& pLDInfo,
                                    ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord target = pReloc.target();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A = helper_sign_extend(target, 31) +
                                  pReloc.addend();
  ARMRelocationFactory::Address S;

  S = pReloc.symValue();
  // if symbol has plt
  if ( pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT) {
    S = helper_PLT(pReloc, pParent);
    T = 0;  // PLT is not thumb.
  }

  ARMRelocationFactory::DWord X = (S + A) | T ;
  pReloc.target() = helper_bit_select(target, X, 0x7fffffffU);
  if (helper_check_signed_overflow(X, 31))
    return ARMRelocationFactory::Overflow;
  return ARMRelocationFactory::OK;
}

// R_ARM_TLS_GD32: GOT(S) + A - P
// R_ARM_TLS_IE32: GOT(S) + A - P
// R_ARM_TLS_LE32: S + A - tp
ARMRelocationFactory::Result tls(Relocation& pReloc,
                                 const MCLDInfo& pLDInfo,
                                 ARMRelocationFactory& pParent)
{
  return ARMRelocationFactory::Unsupport;
}

ARMRelocationFactory::Result unsupport(Relocation& pReloc,
                                       const MCLDInfo& pLDInfo,
                                       ARMRelocationFactory& pParent)
{
  return ARMRelocationFactory::Unsupport;
}
