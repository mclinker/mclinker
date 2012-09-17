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
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/LinkerConfig.h>
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
X86RelocationFactory::applyRelocation(Relocation& pRelocation)
{
  Relocation::Type type = pRelocation.type();

  if (type >= sizeof (ApplyFunctions) / sizeof (ApplyFunctions[0]) ) {
    return Unknown;
  }

  // apply the relocation
  return ApplyFunctions[type].func(pRelocation, *this);
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
                          const X86RelocationFactory& pFactory)

{
  // if symbol is dynamic or undefine or preemptible
  if (pSym.isDyn() ||
      pSym.isUndef() ||
      pFactory.getTarget().isSymbolPreemptible(pSym))
    return false;
  return true;
}

static
GOT::Entry& helper_get_GOT_and_init(Relocation& pReloc,
                                  X86RelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  X86GNULDBackend& ld_backend = pParent.getTarget();

  GOT::Entry* got_entry = pParent.getSymGOTMap().lookUp(*rsym);
  if (NULL != got_entry)
    return *got_entry;

  // not found
  got_entry = ld_backend.getGOT().consume();
  pParent.getSymGOTMap().record(*rsym, *got_entry);

  // If we first get this GOT entry, we should initialize it.
  if (rsym->reserved() & X86GNULDBackend::ReserveGOT) {
    // No corresponding dynamic relocation, initialize to the symbol value.
    got_entry->setContent(pReloc.symValue());
  }
  else if (rsym->reserved() & X86GNULDBackend::GOTRel) {
    // Initialize corresponding dynamic relocation.
    Relocation& rel_entry = *ld_backend.getRelDyn().consumeEntry();
    if (helper_use_relative_reloc(*rsym, pParent)) {
      // Initialize got entry to target symbol address
      got_entry->setContent(pReloc.symValue());
      rel_entry.setType(llvm::ELF::R_386_RELATIVE);
      rel_entry.setSymInfo(0);
    }
    else {
      got_entry->setContent(0);
      rel_entry.setType(llvm::ELF::R_386_GLOB_DAT);
      rel_entry.setSymInfo(rsym);
    }
    rel_entry.targetRef().assign(*got_entry);
  }
  else {
    fatal(diag::reserve_entry_number_mismatch_got);
  }
  return *got_entry;
}


static
X86RelocationFactory::Address helper_GOT_ORG(X86RelocationFactory& pParent)
{
  return pParent.getTarget().getGOTPLT().addr();
}


static
X86RelocationFactory::Address helper_GOT(Relocation& pReloc,
                                         X86RelocationFactory& pParent)
{
  GOT::Entry& got_entry = helper_get_GOT_and_init(pReloc, pParent);
  X86RelocationFactory::Address got_addr =
    pParent.getTarget().getGOT().addr();
  return got_addr +
            pParent.getFragmentLinker().getLayout().getOutputOffset(got_entry);
}


static
PLT::Entry& helper_get_PLT_and_init(Relocation& pReloc,
                                  X86RelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  X86GNULDBackend& ld_backend = pParent.getTarget();

  PLT::Entry* plt_entry = pParent.getSymPLTMap().lookUp(*rsym);
  if (NULL != plt_entry)
    return *plt_entry;

  // not found
  plt_entry = ld_backend.getPLT().consume();
  pParent.getSymPLTMap().record(*rsym, *plt_entry);
  // If we first get this PLT entry, we should initialize it.
  if (rsym->reserved() & X86GNULDBackend::ReservePLT) {
    GOT::Entry* gotplt_entry = pParent.getSymGOTPLTMap().lookUp(*rsym);
    assert(NULL == gotplt_entry && "PLT entry not exist, but DynRel entry exist!");
    gotplt_entry = ld_backend.getGOTPLT().consume();
    pParent.getSymGOTPLTMap().record(*rsym, *gotplt_entry);

    Relocation& rel_entry = *ld_backend.getRelPLT().consumeEntry();
    rel_entry.setType(llvm::ELF::R_386_JUMP_SLOT);
    rel_entry.targetRef().assign(*gotplt_entry);
    rel_entry.setSymInfo(rsym);
  }
  else {
    fatal(diag::reserve_entry_number_mismatch_plt);
  }

  return *plt_entry;
}

static
X86RelocationFactory::Address helper_PLT_ORG(X86RelocationFactory& pParent)
{
  return pParent.getTarget().getPLT().addr();
}


static
X86RelocationFactory::Address helper_PLT(Relocation& pReloc,
                                         X86RelocationFactory& pParent)
{
  PLT::Entry& plt_entry = helper_get_PLT_and_init(pReloc, pParent);
  return helper_PLT_ORG(pParent) +
            pParent.getFragmentLinker().getLayout().getOutputOffset(plt_entry);
}

// Get an relocation entry in .rel.dyn
static
Relocation& helper_DynRel(ResolveInfo* pSym,
                          Fragment& pFrag,
                          uint64_t pOffset,
                          X86RelocationFactory::Type pType,
                          X86RelocationFactory& pParent)
{
  X86GNULDBackend& ld_backend = pParent.getTarget();
  Relocation& rel_entry = *ld_backend.getRelDyn().consumeEntry();
  rel_entry.setType(pType);
  rel_entry.targetRef().assign(pFrag, pOffset);

  if (pType == llvm::ELF::R_386_RELATIVE || NULL == pSym)
    rel_entry.setSymInfo(0);
  else
    rel_entry.setSymInfo(pSym);

  return rel_entry;
}


//=========================================//
// Each relocation function implementation //
//=========================================//

// R_386_NONE
X86RelocationFactory::Result none(Relocation& pReloc,
                                  X86RelocationFactory& pParent)
{
  return X86RelocationFactory::OK;
}

// R_386_32: S + A
X86RelocationFactory::Result abs32(Relocation& pReloc,
                                   X86RelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  RelocationFactory::DWord S = pReloc.symValue();
  bool has_dyn_rel = pParent.getTarget().symbolNeedsDynRel(
                              pParent.getFragmentLinker(),
                              *rsym,
                              (rsym->reserved() & X86GNULDBackend::ReservePLT),
                              true);

  const LDSection* target_sect =
                    pParent.getFragmentLinker().getLayout().getOutputLDSection(
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
    helper_DynRel(rsym, *pReloc.targetRef().frag(), pReloc.targetRef().offset(),
                                            llvm::ELF::R_386_RELATIVE, pParent);
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
      if (helper_use_relative_reloc(*rsym, pParent)) {
        helper_DynRel(rsym, *pReloc.targetRef().frag(),
              pReloc.targetRef().offset(), llvm::ELF::R_386_RELATIVE, pParent);
      }
      else {
        helper_DynRel(rsym, *pReloc.targetRef().frag(),
                          pReloc.targetRef().offset(), pReloc.type(), pParent);
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
                                   X86RelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  RelocationFactory::DWord S = pReloc.symValue();
  RelocationFactory::DWord P =
                         pReloc.place(pParent.getFragmentLinker().getLayout());

  const LDSection* target_sect =
                    pParent.getFragmentLinker().getLayout().getOutputLDSection(
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
                              pParent.getFragmentLinker(),
                              *rsym,
                              (rsym->reserved() & X86GNULDBackend::ReservePLT),
                              false)) {
      if (helper_use_relative_reloc(*rsym, pParent) ) {
        helper_DynRel(rsym, *pReloc.targetRef().frag(),
              pReloc.targetRef().offset(), llvm::ELF::R_386_RELATIVE, pParent);
      }
      else {
        helper_DynRel(rsym, *pReloc.targetRef().frag(),
                          pReloc.targetRef().offset(), pReloc.type(), pParent);
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
                                      X86RelocationFactory& pParent)
{
  RelocationFactory::DWord      A = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
  X86RelocationFactory::Address S = pReloc.symValue();

  pReloc.target() = S + A - GOT_ORG;
  return X86RelocationFactory::OK;
}

// R_386_GOTPC: GOT_ORG + A - P
X86RelocationFactory::Result gotpc32(Relocation& pReloc,
                                     X86RelocationFactory& pParent)
{
  RelocationFactory::DWord      A       = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
  // Apply relocation.
  pReloc.target() = GOT_ORG + A -
                         pReloc.place(pParent.getFragmentLinker().getLayout());
  return X86RelocationFactory::OK;
}

// R_386_GOT32: GOT(S) + A - GOT_ORG
X86RelocationFactory::Result got32(Relocation& pReloc,
                                   X86RelocationFactory& pParent)
{
  if (!(pReloc.symInfo()->reserved()
       & (X86GNULDBackend::ReserveGOT |X86GNULDBackend::GOTRel))) {
    return X86RelocationFactory::BadReloc;
  }
  X86RelocationFactory::Address GOT_S   = helper_GOT(pReloc, pParent);
  RelocationFactory::DWord      A       = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address GOT_ORG = helper_GOT_ORG(pParent);
  // Apply relocation.
  pReloc.target() = GOT_S + A - GOT_ORG;
  return X86RelocationFactory::OK;
}

// R_386_PLT32: PLT(S) + A - P
X86RelocationFactory::Result plt32(Relocation& pReloc,
                                   X86RelocationFactory& pParent)
{
  // PLT_S depends on if there is a PLT entry.
  X86RelocationFactory::Address PLT_S;
  if ((pReloc.symInfo()->reserved() & X86GNULDBackend::ReservePLT))
    PLT_S = helper_PLT(pReloc, pParent);
  else
    PLT_S = pReloc.symValue();
  RelocationFactory::DWord      A = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address P =
                         pReloc.place(pParent.getFragmentLinker().getLayout());
  pReloc.target() = PLT_S + A - P;
  return X86RelocationFactory::OK;
}

// R_386_TLS_GD:
X86RelocationFactory::Result tls_gd(Relocation& pReloc,
                                    X86RelocationFactory& pParent)
{
  // global-dynamic
  ResolveInfo* rsym = pReloc.symInfo();
  // must reserve two pairs of got and dynamic relocation
  if (!(rsym->reserved() & X86GNULDBackend::GOTRel)) {
     return X86RelocationFactory::BadReloc;
  }

  X86GNULDBackend& ld_backend = pParent.getTarget();
  ELFFileFormat* file_format = pParent.getTarget().getOutputFormat();
  // setup corresponding got and dynamic relocatio entries:
  // get first got entry, if there is already a got entry for rsym, then apply
  // this relocation to the got entry directly. If not, setup the corresponding
  // got and dyn relocation entries
  GOT::Entry* got_entry1 = pParent.getSymGOTMap().lookUp(*rsym);

  if (NULL == got_entry1) {
    // get and init two got entries if not exist
    got_entry1 = ld_backend.getGOT().consume();
    pParent.getSymGOTMap().record(*rsym, *got_entry1);
    GOT::Entry* got_entry2 = ld_backend.getGOT().consume();
    got_entry1->setContent(0x0);
    got_entry2->setContent(0x0);
    // setup dyn rel for get_entry1
    Relocation& rel_entry1 = helper_DynRel(rsym, *got_entry1, 0x0,
                                        llvm::ELF::R_386_TLS_DTPMOD32, pParent);
    if (rsym->isLocal()) {
      // for local symbol, set got_entry2 to symbol value
      got_entry2->setContent(pReloc.symValue());

      // for local tls symbol, add rel entry against the section symbol this
      // symbol belong to (.tdata or .tbss)
      const LDSection* sym_sect =
         &rsym->outSymbol()->fragRef()->frag()->getParent()->getSection();
      ResolveInfo* sect_sym = NULL;
      if (&file_format->getTData() == sym_sect)
        sect_sym = pParent.getTarget().getTDATASymbol().resolveInfo();
      else
        sect_sym = pParent.getTarget().getTBSSSymbol().resolveInfo();
      rel_entry1.setSymInfo(sect_sym);
    }
    else {
      // for non-local symbol, add a pair of rel entries against this symbol
      // for those two got entries
      Relocation& rel_entry2 = helper_DynRel(rsym, *got_entry2, 0x0,
                                        llvm::ELF::R_386_TLS_DTPOFF32, pParent);
    }
  }

  // perform relocation to the first got entry
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  // GOT_OFF - the offset between the got_entry1 and _GLOBAL_OFFSET_TABLE (the
  // .got.plt section)
  X86RelocationFactory::Address GOT_OFF =
     file_format->getGOT().addr() +
     pParent.getFragmentLinker().getLayout().getOutputOffset(*got_entry1) -
     file_format->getGOTPLT().addr();
  pReloc.target() = GOT_OFF + A;
  return X86RelocationFactory::OK;
}

// R_X86_TLS_IE
X86RelocationFactory::Result tls_ie(Relocation& pReloc,
                                    X86RelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  if (!(rsym->reserved() & X86GNULDBackend::GOTRel)) {
     return X86RelocationFactory::BadReloc;
  }

  if (rsym->reserved() & X86GNULDBackend::ReserveRel) {
    // when building shared object, set up a RELATIVE dynamic relocation
    helper_DynRel(rsym, *pReloc.targetRef().frag(), pReloc.targetRef().offset(),
                                            llvm::ELF::R_386_RELATIVE, pParent);
  }

  // set up the got and dynamic relocation entries if not exist
  GOT::Entry* got_entry = pParent.getSymGOTMap().lookUp(*rsym);
  if (NULL == got_entry) {
    // set got entry
    X86GNULDBackend& ld_backend = pParent.getTarget();
    got_entry = ld_backend.getGOT().consume();
    pParent.getSymGOTMap().record(*rsym, *got_entry);
    got_entry->setContent(0x0);
    // set relocation entry
    Relocation& rel_entry = *ld_backend.getRelDyn().consumeEntry();
    rel_entry.setType(llvm::ELF::R_386_TLS_TPOFF);
    rel_entry.setSymInfo(rsym);
    rel_entry.targetRef().assign(*got_entry);
  }

  // perform relocation to the absolute address of got_entry
  X86RelocationFactory::Address GOT_S = pParent.getTarget().getGOT().addr() +
             pParent.getFragmentLinker().getLayout().getOutputOffset(*got_entry);
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  pReloc.target() = GOT_S + A;

  return X86RelocationFactory::OK;
}

// R_X86_TLS_LE
X86RelocationFactory::Result tls_le(Relocation& pReloc,
                                    X86RelocationFactory& pParent)
{
  ResolveInfo* rsym = pReloc.symInfo();
  if (pReloc.symInfo()->reserved() & X86GNULDBackend::ReserveRel) {
    Relocation& rel_entry = helper_DynRel(rsym,
                                          *pReloc.targetRef().frag(),
                                          pReloc.targetRef().offset(),
                                          llvm::ELF::R_386_TLS_TPOFF,
                                          pParent);
    return X86RelocationFactory::OK;
  }

  // perform static relocation
  // get TLS segment
  ELFSegment* tls_seg = pParent.getTarget().elfSegmentTable().find(
                                       llvm::ELF::PT_TLS, llvm::ELF::PF_R, 0x0);
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address S = pReloc.symValue();
  pReloc.target() = S + A - tls_seg->memsz();
  return X86RelocationFactory::OK;
}

X86RelocationFactory::Result unsupport(Relocation& pReloc,
                                       X86RelocationFactory& pParent)
{
  return X86RelocationFactory::Unsupport;
}

