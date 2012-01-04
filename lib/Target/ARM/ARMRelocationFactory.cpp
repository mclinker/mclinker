//===- ARMRelocationFactory.cpp  ----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/DataTypes.h>
#include <llvm/Support/ELF.h>

#include <mcld/LD/Layout.h>

#include "ARMRelocationFactory.h"
#include "ARMRelocationFunctions.h"

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
uint64_t helper_bit_select(uint64_t pA, uint64_t pB, uint64_t pMask)
{
  return (pA & ~pMask) | (pB & pMask) ;
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
      Relocation& rel_entry =
        *ld_backend.getRelDyn().getEntry(*rsym, true, exist);
      assert(!exist && "GOT entry not exist, but DynRel entry exist!");
      rel_entry.setType(llvm::ELF::R_ARM_GLOB_DAT);
      rel_entry.targetRef().assign(got_entry);
      rel_entry.setSymInfo(rsym);
    }
    else {
      llvm::report_fatal_error("No GOT entry reserved for GOT type relocation!");
    }
  }
  return got_entry;
}


static
ARMRelocationFactory::Address helper_GOT_ORG(ARMRelocationFactory& pParent)
{
  const LDSection& ld_section = static_cast<const LDSection&>(
    pParent.getTarget().getGOT().getSectionData().getSection()
  );
  return ld_section.offset();
}


static
ARMRelocationFactory::Address helper_GOT(Relocation& pReloc,
                                         ARMRelocationFactory& pParent)
{
  GOTEntry& got_entry = helper_get_GOT_and_init(pReloc, pParent);
  return helper_GOT_ORG(pParent) + pParent.getLayout().getFragmentOffset(got_entry);
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
      llvm::report_fatal_error("No PLT entry reserved for PLT type relocation!");
    }
  }
  return plt_entry;
}



static
ARMRelocationFactory::Address helper_PLT_ORG(ARMRelocationFactory& pParent)
{
  const LDSection& ld_section = static_cast<const LDSection&>(
    pParent.getTarget().getPLT().getSectionData().getSection()
  );
  return ld_section.offset();
}


static
ARMRelocationFactory::Address helper_PLT(Relocation& pReloc,
                                         ARMRelocationFactory& pParent)
{
  PLTEntry& plt_entry = helper_get_PLT_and_init(pReloc, pParent);
  return helper_PLT_ORG(pParent) + pParent.getLayout().getFragmentOffset(plt_entry);
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
  // imm16: [19-16][26][14-12][7-0]
  return helper_sign_extend((((pTarget >> 4) & 0xf000U) |
                             ((pTarget >> 15) & 0x0800U) |
                             ((pTarget >> 4) & 0x0700U) |
                             (pTarget & 0x00ffU)),
                            16);
}

static ARMRelocationFactory::DWord
helper_insert_val_thumb_movw_movt_inst(ARMRelocationFactory::DWord pTarget,
                                       ARMRelocationFactory::DWord pImm)
{
  // imm16: [19-16][26][14-12][7-0]
  pTarget &= 0xfbf08f00U;
  pTarget |= (pImm & 0xf000U) << 4;
  pTarget |= (pImm & 0x0800U) << 15;
  pTarget |= (pImm & 0x0700U) << 4;
  pTarget |= (pImm & 0x00ffU);
  return pTarget;
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

// Check if symbol can use relocation R_ARM_RELATIVE
// Only need to check R_ARM_ABS32 and R_ARM_ABS32_NOI
static bool
helper_use_relative_reloc(ResolveInfo& pSym)
{
  // if symbol has plt, should use RELATIVE
  if(pSym.reserved() & 0x8u)
    return true;
  // if symbol is dynamic or undefine or preemptible
  if(pSym.isDyn() || pSym.isUndef() || pSym.other() == ResolveInfo::Default)
    return false;

  return true;
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
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  ARMRelocationFactory::DWord S = pReloc.symValue();

  if(rsym->isLocal() && (rsym->reserved() & 0x1u)) {
    helper_DynRel(pReloc, llvm::ELF::R_ARM_RELATIVE, pParent);
    pReloc.target() = (S + A) | T ;
    return ARMRelocationFactory::OK;
  }
  else if(rsym->isGlobal()) {
    if(rsym->reserved() & 0x8u) {
      S = helper_PLT(pReloc, pParent);
      T = 0 ; // PLT is not thumb
      pReloc.target() = (S + A) | T;
    }
    if(rsym->reserved() & 0x1u) {
      if(helper_use_relative_reloc(*rsym))
        helper_DynRel(pReloc, llvm::ELF::R_ARM_RELATIVE, pParent);
      else
        helper_DynRel(pReloc, pReloc.type(), pParent);
    }
    return ARMRelocationFactory::OK;
  }

  // perform static relocation
  pReloc.target() = (S + A) | T;
  return ARMRelocationFactory::OK;
}

// R_ARM_REL32: ((S + A) | T) - P
ARMRelocationFactory::Result rel32(Relocation& pReloc,
                                   ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  // if symbol needs dynamic relocation
  if(rsym->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_REL32 ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
  }
  // perform static relocation
  else {
    ARMRelocationFactory::DWord T = getThumbBit(pReloc);
    ARMRelocationFactory::DWord A = pReloc.target() + pReloc.addend();
    pReloc.target() = ((pReloc.symValue() + A) | T)
      - pReloc.place(pParent.getLayout());
  }
  return ARMRelocationFactory::OK;
}

// R_ARM_GOTOFF32: ((S + A) | T) - GOT_ORG
ARMRelocationFactory::Result gotoff32(Relocation& pReloc,
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
                                      ARMRelocationFactory& pParent)
{
  switch (pReloc.symInfo()->reserved()) {
  default:
    return ARMRelocationFactory::BadReloc;

  // Only allow these four reserved entry type.
  case ARMGNULDBackend::ReserveGOT:
  case ARMGNULDBackend::GOTRel:
  case ARMGNULDBackend::GOTandRel:
  case ARMGNULDBackend::GOTRelandRel:
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
  assert(!helper_check_signed_overflow(X, 24) && "Jump or Call target too far!");
  //                    Make sure the Imm is 0.          Result Mask.
  pReloc.target() = (pReloc.target() & 0xFF000000u) | ((X & 0x03FFFFFEu) >> 2);
  return ARMRelocationFactory::OK;
}

// R_ARM_THM_CALL: ((S + A) | T) - P
ARMRelocationFactory::Result thm_call(Relocation& pReloc,
                                      ARMRelocationFactory& pParent)
{
  // If target is undefined weak symbol, we only need to jump to the
  // next instruction unless it has PLT entry.
  if (pReloc.symInfo()->isWeak() && pReloc.symInfo()->isUndef() &&
      !(pReloc.symInfo()->reserved() & ARMGNULDBackend::ReservePLT)) {
    pReloc.target() = (0xe000U << 16) | 0xbf00U;
    return ARMRelocationFactory::OK;
  }

  ARMRelocationFactory::DWord upper16 = ((pReloc.target() & 0xffff0000U) >> 16),
                              lower16 = (pReloc.target() & 0xffffU);

  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A = helper_thumb32_branch_offset(upper16,
                                                               lower16);
  ARMRelocationFactory::Address P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::Address S;

  switch (pReloc.symInfo()->reserved()) {
    default: {
      return ARMRelocationFactory::BadReloc;
    }
    case ARMGNULDBackend::None: {
      S = pReloc.symValue();
      break;
    }
    case ARMGNULDBackend::ReservePLT: {
      S = helper_PLT(pReloc, pParent);
      T = 1;  // PLT is in thumb mode.
      break;
    }
  }

  ARMRelocationFactory::DWord X = ((S + A) | T) - P;
  X >>= 1;

  // FIXME: Check bit size is 24(thumb2) or 22?
  if (helper_check_signed_overflow(X, 24)) {
    assert(!"Offset is too far. We need stub or PLT for it.");
    return ARMRelocationFactory::Overflow;
  }

  // For a BLX instruction, make sure that the relocation is rounded up
  // to a word boundary. This follows the semantics of the instruction
  // which specifies that bit 1 of the target address will come from bit
  // 1 of the base address.
  if ((X & 0x5000U) == 0x4000U) {
    X = (X + 2) & ~0x3U;
  }

  upper16 = helper_thumb32_branch_upper(upper16, X);
  lower16 = helper_thumb32_branch_lower(lower16, X);

  pReloc.target() = (upper16 << 16);
  pReloc.target() |= lower16;

  return ARMRelocationFactory::OK;
}

// R_ARM_MOVW_ABS_NC: (S + A) | T
ARMRelocationFactory::Result movw_abs_nc(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  // use plt
  if(rsym->reserved() & 0x8u) {
    S = helper_PLT(pReloc, pParent);
    T = 0 ; // PLT is not thumb
  }
  // use dynamic relocation
  if(rsym->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_MOVT_ABS_NC ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
  }
  X = (S + A) | T ;
  // perform static relocation
  pReloc.target() = (S + A) | T;
  if (helper_check_signed_overflow(X, 16)) {
    return ARMRelocationFactory::Overflow;
  } else {
    pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
    return ARMRelocationFactory::OK;
  }
}

// R_ARM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocationFactory::Result movw_prel_nc(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  // use dynamic relocation
  if(rsym->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_MOVT_PREL_NC ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
  }
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
                                      ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
    helper_extract_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  // use plt
  if(rsym->reserved() & 0x8u) {
    S = helper_PLT(pReloc, pParent);
  }
  // use dynamic relocation
  if(rsym->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_MOVT_ABS ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
  }

  X = S + A;
  X >>= 16;
  // perform static relocation
  pReloc.target() = helper_insert_val_movw_movt_inst(pReloc.target(), X);
  return ARMRelocationFactory::OK;
}

// R_ARM_MOVT_PREL: S + A - P
ARMRelocationFactory::Result movt_prel(Relocation& pReloc,
                                       ARMRelocationFactory& pParent)
{
  // use dynamic relocation
  if(pReloc.symInfo()->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_MOVT_PREL ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
  }

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
                                             ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  // use plt
  if(rsym->reserved() & 0x8u) {
    S = helper_PLT(pReloc, pParent);
    T = 0; // PLT is not thumb
  }
  // use dynamic relocation
  if(rsym->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_THM_MOVW_ABS_NC ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
  }

  X = (S + A) | T;
  // check 16-bit overflow
  if (helper_check_signed_overflow(X, 16)) {
    return ARMRelocationFactory::Overflow;
  } else {
    pReloc.target() = helper_insert_val_thumb_movw_movt_inst(pReloc.target(),
                                                             X);
    return ARMRelocationFactory::OK;
  }
}

// R_ARM_THM_MOVW_PREL_NC: ((S + A) | T) - P
ARMRelocationFactory::Result thm_movw_prel_nc(Relocation& pReloc,
                                      ARMRelocationFactory& pParent)
{
  // use dynamic relocation
  if(pReloc.symInfo()->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_THM_MOVW_PREL_NC ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
  }

  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  X = ((S + A) | T) - P;

  // check 16-bit overflow
  if (helper_check_signed_overflow(X, 16)) {
    return ARMRelocationFactory::Overflow;
  } else {
    pReloc.target() = helper_insert_val_thumb_movw_movt_inst(pReloc.target(),
                                                             X);
    return ARMRelocationFactory::OK;
  }
}

// R_ARM_THM_MOVT_ABS: S + A
ARMRelocationFactory::Result thm_movt_abs(Relocation& pReloc,
                                          ARMRelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  // use plt
  if(rsym->reserved() & 0x8u) {
    S = helper_PLT(pReloc, pParent);
  }
  // use dynamic relocation
  if(rsym->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_THM_MOVT_ABS ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
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
ARMRelocationFactory::Result thm_movt_prel(Relocation& pReloc,
                                           ARMRelocationFactory& pParent)
{
  // use dynamic relocation
  if(pReloc.symInfo()->reserved() & 0x1u) {
    llvm::report_fatal_error(llvm::Twine("Requires unsupported dynamic ") +
                             llvm::Twine("relocation R_ARM_THM_MOVT_PREL ") +
                             llvm::Twine("for symbol ") +
                             llvm::Twine(pReloc.symInfo()->name()) +
                             llvm::Twine(", recompile with -fPIC"));
  }

  ARMRelocationFactory::Address S = pReloc.symValue();
  ARMRelocationFactory::DWord P = pReloc.place(pParent.getLayout());
  ARMRelocationFactory::DWord A =
      helper_extract_thumb_movw_movt_addend(pReloc.target()) + pReloc.addend();
  ARMRelocationFactory::DWord X;

  X = S + A - P;
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

// R_ARM_PREL31: (S + A) | T
ARMRelocationFactory::Result prel31(Relocation& pReloc,
                                  ARMRelocationFactory& pParent)
{
  ARMRelocationFactory::DWord target = pReloc.target();
  ARMRelocationFactory::DWord T = getThumbBit(pReloc);
  ARMRelocationFactory::DWord A = helper_sign_extend(target, 31) +
                                  pReloc.addend();
  ARMRelocationFactory::Address S;

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

  ARMRelocationFactory::DWord X = (S + A) | T ;
  pReloc.target() = helper_bit_select(target, X, 0x7fffffffU);
  if(helper_check_signed_overflow(X, 31))
    return ARMRelocationFactory::Overflow;
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

