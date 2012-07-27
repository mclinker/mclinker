//===- X86RelocationFactory.cpp -------------------------------------------===//
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
#include <mcld/MC/MCLDInfo.h>
#include <mcld/LD/Layout.h>
#include <mcld/Support/MsgHandling.h>

#include "X86RelocationFactory.h"
#include "X86RelocationFunctions.h"

using namespace mcld;

//===--------------------------------------------------------------------===//
// Relocation Functions and Tables
//===--------------------------------------------------------------------===//
DECL_X86_APPLY_RELOC_FUNCS

/// the prototype of applying function
typedef RelocationFactory::Result
                          (*ApplyFunctionType)(Relocation& pReloc,
                                               const MCLDInfo& pLDInfo,
                                               X86RelocationFactory& pParent);

// the table entry of applying functions
struct ApplyFunctionTriple
{
  ApplyFunctionType func;
  unsigned int type;
  const char* name;
};

// declare the table of applying functions
static const ApplyFunctionTriple ApplyFunctions[] = {
  DECL_X86_APPLY_RELOC_FUNC_PTRS
};

//===--------------------------------------------------------------------===//
// X86RelocationFactory
//===--------------------------------------------------------------------===//
X86RelocationFactory::X86RelocationFactory(size_t pNum,
                                           X86GNULDBackend& pParent)
  : RelocationFactory(pNum),
    m_Target(pParent) {
}

X86RelocationFactory::~X86RelocationFactory()
{
}

RelocationFactory::Result
X86RelocationFactory::applyRelocation(Relocation& pRelocation,
                                           const MCLDInfo& pLDInfo)
{
  Relocation::Type type = pRelocation.type();

  if (type >= sizeof (ApplyFunctions) / sizeof (ApplyFunctions[0]) ) {
    fatal(diag::unknown_relocation) << (int)type <<
                                       pRelocation.symInfo()->name();
    return Unknown;
  }

  // apply the relocation
  return ApplyFunctions[type].func(pRelocation, pLDInfo, *this);
}

const char* X86RelocationFactory::getName(Relocation::Type pType) const
{
  return ApplyFunctions[pType].name;
}

//===--------------------------------------------------------------------===//
// Relocation helper function
//===--------------------------------------------------------------------===//

// Check if symbol can use relocation R_386_RELATIVE
static bool
helper_use_relative_reloc(const ResolveInfo& pSym,
                          const MCLDInfo& pLDInfo,
                          const X86RelocationFactory& pFactory)

{
  // if symbol is dynamic or undefine or preemptible
  if (pSym.isDyn() ||
     pSym.isUndef() ||
     pFactory.getTarget().isSymbolPreemptible(pSym, pLDInfo, pLDInfo.output()))
    return false;
  return true;
}

static
GOTEntry& helper_get_GOT_and_init(Relocation& pReloc,
                                  const MCLDInfo& pLDInfo,
                                  X86RelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  X86GNULDBackend& ld_backend = pParent.getTarget();

  bool exist;
  GOTEntry& got_entry = *ld_backend.getGOT().getEntry(*rsym, exist);
  if (!exist) {
    // If we first get this GOT entry, we should initialize it.
    if (rsym->reserved() & X86GNULDBackend::ReserveGOT) {
      // No corresponding dynamic relocation, initialize to the symbol value.
      got_entry.setContent(pReloc.symValue());
    }
    else if (rsym->reserved() & X86GNULDBackend::GOTRel) {
      // Initialize corresponding dynamic relocation.
      Relocation& rel_entry =
        *ld_backend.getRelDyn().getEntry(*rsym, true, exist);
      assert(!exist && "GOT entry not exist, but DynRel entry exist!");
      if (helper_use_relative_reloc(*rsym, pLDInfo, pParent)) {
        // Initialize got entry to target symbol address
        got_entry.setContent(pReloc.symValue());
        rel_entry.setType(llvm::ELF::R_386_RELATIVE);
        rel_entry.setSymInfo(0);
      }
      else {
        got_entry.setContent(0);
        rel_entry.setType(llvm::ELF::R_386_GLOB_DAT);
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
X86RelocationFactory::Address helper_GOT_ORG(X86RelocationFactory& pParent)
{
  return pParent.getTarget().getGOTPLT().getSection().addr();
}


static
X86RelocationFactory::Address helper_GOT(Relocation& pReloc,
                                         const MCLDInfo& pLDInfo,
                                         X86RelocationFactory& pParent)
{
  GOTEntry& got_entry = helper_get_GOT_and_init(pReloc, pLDInfo,  pParent);
  X86RelocationFactory::Address got_addr =
    pParent.getTarget().getGOT().getSection().addr();
  return got_addr + pParent.getLayout().getOutputOffset(got_entry);
}


static
PLTEntry& helper_get_PLT_and_init(Relocation& pReloc,
                                  X86RelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  X86GNULDBackend& ld_backend = pParent.getTarget();

  bool exist;
  PLTEntry& plt_entry = *ld_backend.getPLT().getPLTEntry(*rsym, exist);
  if (!exist) {
    // If we first get this PLT entry, we should initialize it.
    if (rsym->reserved() & X86GNULDBackend::ReservePLT) {
      GOTEntry& gotplt_entry =
        *ld_backend.getPLT().getGOTPLTEntry(*rsym, exist);
      // Initialize corresponding dynamic relocation.
      Relocation& rel_entry =
        *ld_backend.getRelPLT().getEntry(*rsym, true, exist);
      assert(!exist && "PLT entry not exist, but DynRel entry exist!");
      rel_entry.setType(llvm::ELF::R_386_JUMP_SLOT);
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
X86RelocationFactory::Address helper_PLT_ORG(X86RelocationFactory& pParent)
{
  return pParent.getTarget().getPLT().getSection().addr();
}


static
X86RelocationFactory::Address helper_PLT(Relocation& pReloc,
                                         X86RelocationFactory& pParent)
{
  PLTEntry& plt_entry = helper_get_PLT_and_init(pReloc, pParent);
  return helper_PLT_ORG(pParent) + pParent.getLayout().getOutputOffset(plt_entry);
}

// Get an relocation entry in .rel.dyn and set its type to pType,
// its FragmentRef to pReloc->targetFrag() and its ResolveInfo to pReloc->symInfo()
static
void helper_DynRel(Relocation& pReloc,
                   X86RelocationFactory::Type pType,
                   X86RelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  X86GNULDBackend& ld_backend = pParent.getTarget();
  bool exist;

  Relocation& rel_entry =
    *ld_backend.getRelDyn().getEntry(*rsym, false, exist);
  rel_entry.setType(pType);
  rel_entry.targetRef() = pReloc.targetRef();

  if (pType == llvm::ELF::R_386_RELATIVE)
    rel_entry.setSymInfo(0);
  else
    rel_entry.setSymInfo(rsym);
}


//=========================================//
// Each relocation function implementation //
//=========================================//

// R_386_NONE
X86RelocationFactory::Result none(Relocation& pReloc,
                                  const MCLDInfo& pLDInfo,
                                  X86RelocationFactory& pParent)
{
  return X86RelocationFactory::OK;
}

// R_386_32: S + A
X86RelocationFactory::Result abs32(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   X86RelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  RelocationFactory::DWord S = pReloc.symValue();
  bool has_dyn_rel = pParent.getTarget().symbolNeedsDynRel(
                       *rsym, (rsym->reserved() & X86GNULDBackend::ReservePLT),
                       pLDInfo, pLDInfo.output(), true);

  const LDSection* target_sect = pParent.getLayout().getOutputLDSection(
                                                  *(pReloc.targetRef().frag()));
  assert(NULL != target_sect);
  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 == (llvm::ELF::SHF_ALLOC & target_sect->flag())) {
    pReloc.target() = S + A;
    return X86RelocationFactory::OK;
  }

  // A local symbol may need REL Type dynamic relocation
  if (rsym->isLocal() && has_dyn_rel) {
    helper_DynRel(pReloc, llvm::ELF::R_386_RELATIVE, pParent);
    pReloc.target() = S + A;
    return X86RelocationFactory::OK;
  }

  // An external symbol may need PLT and dynamic relocation
  if (!rsym->isLocal()) {
    if (rsym->reserved() & X86GNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      pReloc.target() = S + A;
    }
    // If we generate a dynamic relocation (except R_386_RELATIVE)
    // for a place, we should not perform static relocation on it
    // in order to keep the addend store in the place correct.
    if (has_dyn_rel) {
      if (helper_use_relative_reloc(*rsym, pLDInfo, pParent)) {
        helper_DynRel(pReloc, llvm::ELF::R_386_RELATIVE, pParent);
      }
      else {
        helper_DynRel(pReloc, pReloc.type(), pParent);
        return X86RelocationFactory::OK;
      }
    }
  }

  // perform static relocation
  pReloc.target() = S + A;
  return X86RelocationFactory::OK;
}

// R_386_PC32: S + A - P
X86RelocationFactory::Result rel32(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   X86RelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  RelocationFactory::DWord S = pReloc.symValue();
  RelocationFactory::DWord P = pReloc.place(pParent.getLayout());

  const LDSection* target_sect = pParent.getLayout().getOutputLDSection(
                                                  *(pReloc.targetRef().frag()));
  assert(NULL != target_sect);
  // If the flag of target section is not ALLOC, we will not scan this relocation
  // but perform static relocation. (e.g., applying .debug section)
  if (0x0 == (llvm::ELF::SHF_ALLOC & target_sect->flag())) {
    pReloc.target() = S + A - P;
    return X86RelocationFactory::OK;
  }

  // An external symbol may need PLT and dynamic relocation
  if (!rsym->isLocal()) {
    if (rsym->reserved() & X86GNULDBackend::ReservePLT) {
       S = helper_PLT(pReloc, pParent);
       pReloc.target() = S + A - P;
    }
    if (pParent.getTarget().symbolNeedsDynRel(
          *rsym, (rsym->reserved() & X86GNULDBackend::ReservePLT), pLDInfo,
                  pLDInfo.output(), false)) {
      if (helper_use_relative_reloc(*rsym, pLDInfo, pParent) ) {
        helper_DynRel(pReloc, llvm::ELF::R_386_RELATIVE, pParent);
      }
      else {
        helper_DynRel(pReloc, pReloc.type(), pParent);
          return X86RelocationFactory::OK;
      }
    }
  }

   // perform static relocation
  pReloc.target() = S + A - P;
  return X86RelocationFactory::OK;
}

// R_386_GOTOFF: S + A - GOT_ORG
X86RelocationFactory::Result gotoff32(Relocation& pReloc,
                                      const MCLDInfo& pLDInfo,
                                      X86RelocationFactory& pParent)
{
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
  X86RelocationFactory::Address S = pReloc.symValue();

  pReloc.target() = S + A - GOT_ORG;
  return X86RelocationFactory::OK;
}

// R_386_GOTPC: GOT_ORG + A - P
X86RelocationFactory::Result gotpc32(Relocation& pReloc,
                                     const MCLDInfo& pLDInfo,
                                     X86RelocationFactory& pParent)
{
  RelocationFactory::DWord   A       = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
  // Apply relocation.
  pReloc.target() = GOT_ORG + A - pReloc.place(pParent.getLayout());
  return X86RelocationFactory::OK;
}

// R_386_GOT32: GOT(S) + A - GOT_ORG
X86RelocationFactory::Result got32(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   X86RelocationFactory& pParent)
{
  if (!(pReloc.symInfo()->reserved()
       & (X86GNULDBackend::ReserveGOT |X86GNULDBackend::GOTRel))) {
    return X86RelocationFactory::BadReloc;
  }
  X86RelocationFactory::Address GOT_S   = helper_GOT(pReloc, pLDInfo, pParent);
  RelocationFactory::DWord   A       = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
  // Apply relocation.
  pReloc.target() = GOT_S + A - GOT_ORG;
  return X86RelocationFactory::OK;
}

// R_386_PLT32: PLT(S) + A - P
X86RelocationFactory::Result plt32(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   X86RelocationFactory& pParent)
{
  // PLT_S depends on if there is a PLT entry.
  X86RelocationFactory::Address PLT_S;
  if ((pReloc.symInfo()->reserved() & X86GNULDBackend::ReservePLT))
    PLT_S = helper_PLT(pReloc, pParent);
  else
    PLT_S = pReloc.symValue();
  RelocationFactory::DWord   A       = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address P = pReloc.place(pParent.getLayout());
  pReloc.target() = PLT_S + A - P;
  return X86RelocationFactory::OK;
}
