//===- X86RelocationFactory.cpp -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/DataTypes.h>
#include <llvm/Support/ELF.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/LD/Layout.h>

#include "X86RelocationFactory.h"
#include "X86RelocationFunctions.h"

using namespace mcld;

DECL_X86_APPLY_RELOC_FUNCS

//===--------------------------------------------------------------------===//
// X86RelocationFactory
X86RelocationFactory::X86RelocationFactory(size_t pNum,
                                           X86GNULDBackend& pParent)
  : RelocationFactory(pNum),
    m_Target(pParent) {
}

X86RelocationFactory::~X86RelocationFactory()
{
}

void X86RelocationFactory::applyRelocation(Relocation& pRelocation,
                                           const MCLDInfo& pLDInfo)
{
  Relocation::Type type = pRelocation.type();

  /// the prototype of applying function
  typedef Result (*ApplyFunctionType)(Relocation& pReloc,
				      const MCLDInfo& pLDInfo,
                                      X86RelocationFactory& pParent);

  // the table entry of applying functions
  struct ApplyFunctionTriple {
    ApplyFunctionType func;
    unsigned int type;
    const char* name;
  };

  // declare the table of applying functions
  static ApplyFunctionTriple apply_functions[] = {
    DECL_X86_APPLY_RELOC_FUNC_PTRS
  };

  if (type >= sizeof (apply_functions) / sizeof (apply_functions[0]) ) {
    llvm::report_fatal_error(llvm::Twine("Unknown relocation type ") +
			     llvm::Twine((int) type) +
			     llvm::Twine(" to symbol `") +
                             pRelocation.symInfo()->name() +
                             llvm::Twine("'."));
    return;
  }

  // apply the relocation
  Result result = apply_functions[type].func(pRelocation, pLDInfo, *this);

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



// non-member functions

//=========================================//
// Relocation helper function              //
//=========================================//

// Check if symbol can use relocation R_386_RELATIVE
static bool
helper_use_relative_reloc(const ResolveInfo& pSym,
                          const MCLDInfo& pLDInfo,
                          const X86RelocationFactory& pFactory)

{
  // if symbol is dynamic or undefine or preemptible
  if(pSym.isDyn() ||
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
      if(helper_use_relative_reloc(*rsym, pLDInfo, pParent)) {
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
      llvm::report_fatal_error("No GOT entry reserved for GOT type relocation!");
    }
  }
  return got_entry;
}


static
X86RelocationFactory::Address helper_GOT_ORG(X86RelocationFactory& pParent)
{
  return pParent.getTarget().getGOT().getSection().addr();
}


static
X86RelocationFactory::Address helper_GOT(Relocation& pReloc,
                                         const MCLDInfo& pLDInfo,
                                         X86RelocationFactory& pParent)
{
  GOTEntry& got_entry = helper_get_GOT_and_init(pReloc, pLDInfo,  pParent);
  return helper_GOT_ORG(pParent) + pParent.getLayout().getOutputOffset(got_entry);
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
      llvm::report_fatal_error("No PLT entry reserved for PLT type relocation!");
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

  if(pType == llvm::ELF::R_386_RELATIVE)
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

  if(rsym->isLocal() && (rsym->reserved() & X86GNULDBackend::ReserveRel)) {
    helper_DynRel(pReloc, llvm::ELF::R_386_RELATIVE, pParent);
    pReloc.target() = S + A;
    return X86RelocationFactory::OK;
  }
  else if(!rsym->isLocal()) {
    if(rsym->reserved() & X86GNULDBackend::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      pReloc.target() = S + A;
    }
    if(rsym->reserved() & X86GNULDBackend::ReserveRel) {
      if(helper_use_relative_reloc(*rsym, pLDInfo, pParent) ) {
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
  // perform static relocation
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  pReloc.target() = pReloc.symValue() + A
      - pReloc.place(pParent.getLayout());
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
  if(!(pReloc.symInfo()->reserved()
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
  if((pReloc.symInfo()->reserved() & X86GNULDBackend::ReservePLT))
    PLT_S = helper_PLT(pReloc, pParent);
  else
    PLT_S = pReloc.symValue();
  RelocationFactory::DWord   A       = pReloc.target() + pReloc.addend();
  X86RelocationFactory::Address P = pReloc.place(pParent.getLayout());
  pReloc.target() = PLT_S + A - P;
  return X86RelocationFactory::OK;
}
