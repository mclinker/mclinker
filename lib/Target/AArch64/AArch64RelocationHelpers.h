//===- AArch64RelocationHelpers.h -----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_AARCH64_AARCH64RELOCATIONHELPERS_H
#define TARGET_AARCH64_AARCH64RELOCATIONHELPERS_H

#include "AArch64Relocator.h"
#include <llvm/Support/Host.h>

namespace mcld {
//===----------------------------------------------------------------------===//
// Relocation helper functions
//===----------------------------------------------------------------------===//
// Return true if overflow
static inline bool
helper_check_signed_overflow(AArch64Relocator::DWord pValue, unsigned bits)
{
  if (bits >= sizeof(int64_t) * 8)
    return false;
  int64_t signed_val = static_cast<int64_t>(pValue);
  int64_t max = (1 << (bits - 1)) - 1;
  int64_t min = -(1 << (bits - 1));
  if (signed_val > max || signed_val < min)
    return true;
  return false;
}

static inline Relocator::Address
helper_get_page_address(Relocator::Address pValue)
{
  return (pValue & ~ (Relocator::Address) 0xFFF);
}

static inline Relocator::Address
helper_get_page_offset(Relocator::Address pValue)
{
  return (pValue & (Relocator::Address) 0xFFF);
}

static inline uint32_t get_mask(uint32_t pValue)
{
  return ((1u << (pValue)) - 1);
}

static inline uint32_t
helper_reencode_adr_imm(uint32_t pInst, uint32_t pImm)
{
  return (pInst & ~((get_mask(2) << 29) | (get_mask(19) << 5)))
      | ((pImm & get_mask(2)) << 29) | ((pImm & (get_mask(19) << 2)) << 3);
}

// Reencode the imm field of add immediate.
static inline uint32_t helper_reencode_add_imm(uint32_t pInst, uint32_t pImm)
{
  return (pInst & ~(get_mask(12) << 10)) | ((pImm & get_mask(12)) << 10);
}

// Encode the 26-bit offset of unconditional branch.
static inline uint32_t
helper_reencode_branch_offset_26(uint32_t pInst, uint32_t pOff)
{
  return (pInst & ~get_mask(26)) | (pOff & get_mask(26));
}

static inline uint32_t helper_get_upper32(AArch64Relocator::DWord pData)
{
  if (llvm::sys::IsLittleEndianHost)
    return pData >> 32;
  return pData & 0xFFFFFFFF;
}

static inline void
helper_put_upper32(uint32_t pData, AArch64Relocator::DWord& pDes)
{
  *(reinterpret_cast<uint32_t*>(&pDes)) = pData;
}

static inline AArch64Relocator::Address
helper_get_PLT_address(ResolveInfo& pSym, AArch64Relocator& pParent)
{
  PLTEntryBase* plt_entry = pParent.getSymPLTMap().lookUp(pSym);
  assert(NULL != plt_entry);
  return pParent.getTarget().getPLT().addr() + plt_entry->getOffset();
}

static inline AArch64PLT1&
helper_PLT_init(Relocation& pReloc, AArch64Relocator& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  AArch64GNULDBackend& ld_backend = pParent.getTarget();
  assert(NULL == pParent.getSymPLTMap().lookUp(*rsym));

  AArch64PLT1* plt_entry = ld_backend.getPLT().create();
  pParent.getSymPLTMap().record(*rsym, *plt_entry);

  // initialize plt and the corresponding gotplt and dyn rel entry.
  assert(NULL == pParent.getSymGOTPLTMap().lookUp(*rsym) &&
         "PLT entry not exist, but DynRel entry exist!");
  AArch64GOTEntry* gotplt_entry = ld_backend.getGOTPLT().createGOTPLT();
  pParent.getSymGOTPLTMap().record(*rsym, *gotplt_entry);

  // init the corresponding rel entry in .rela.plt
  Relocation& rel_entry = *ld_backend.getRelaPLT().create();
  rel_entry.setType(R_AARCH64_JUMP_SLOT);
  rel_entry.targetRef().assign(*gotplt_entry);
  rel_entry.setSymInfo(rsym);
  return *plt_entry;
}

}
#endif
