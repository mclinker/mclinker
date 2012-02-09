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
#include <mcld/LD/Layout.h>
#include <mcld/Target/OutputRelocSection.h>

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
                                      const MCLDInfo& pLDInfo,
                                      MipsRelocationFactory&);

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

//=========================================//
// Relocation helper function              //
//=========================================//

static const char * const GP_DISP_NAME = "_gp_disp";

// Get an relocation entry in .rel.dyn and set its type to R_MIPS_REL32,
// its FragmentRef to pReloc->targetFrag() and its ResolveInfo
// to pReloc->symInfo()
static
void helper_SetRelDynEntry(Relocation& pReloc,
                   MipsRelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  MipsGNULDBackend& ld_backend = pParent.getTarget();

  bool exist;
  Relocation& rel_entry =
    *ld_backend.getRelDyn().getEntry(*rsym, false, exist);

  rel_entry.setType(llvm::ELF::R_MIPS_REL32);
  rel_entry.targetRef() = pReloc.targetRef();
  rel_entry.setSymInfo(0);
}

// Find next R_MIPS_LO16 relocation paired to pReloc.
static
Relocation* helper_FindLo16Reloc(Relocation& pReloc)
{
  Relocation* reloc = static_cast<Relocation*>(pReloc.getNextNode());
  while (NULL != reloc)
  {
    if (llvm::ELF::R_MIPS_LO16 == reloc->type() &&
        reloc->symInfo() == pReloc.symInfo())
      return reloc;

    reloc = static_cast<Relocation*>(reloc->getNextNode());
  }
  return NULL;
}

// Check the symbol is _gp_disp.
static
bool helper_isGpDisp(const Relocation& pReloc)
{
  const ResolveInfo* rsym = pReloc.symInfo();
  return 0 == strcmp(GP_DISP_NAME, rsym->name());
}

static
RelocationFactory::Address helper_GetGOTAddr(MipsRelocationFactory& pParent)
{
  return pParent.getTarget().getGOT().getSection().addr();
}

static
GOTEntry& helper_GetGOTEntry(Relocation& pReloc,
                             MipsRelocationFactory& pParent)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  MipsGNULDBackend& ld_backend = pParent.getTarget();

  bool exist;
  GOTEntry& got_entry = *ld_backend.getGOT().getEntry(*rsym, exist);

  if (exist)
    return got_entry;

  // If we first get this GOT entry, we should initialize it.
  if (rsym->reserved() & MipsGNULDBackend::ReserveGot) {
    got_entry.setContent(pReloc.symValue());
  }
  else {
    llvm::report_fatal_error("No GOT entry reserved for GOT type relocation!");
  }

  return got_entry;
}

static
RelocationFactory::Address helper_GetGOTOffset(Relocation& pReloc,
                                               MipsRelocationFactory& pParent)
{
  GOTEntry& got_entry = helper_GetGOTEntry(pReloc, pParent);
  return pParent.getLayout().getOutputOffset(got_entry);
}

//=========================================//
// Relocation functions implementation     //
//=========================================//

// R_MIPS_NONE and those unsupported/deprecated relocation type
static
MipsRelocationFactory::Result none(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   MipsRelocationFactory& pParent)
{
  return MipsRelocationFactory::OK;
}

// R_MIPS_32: S + A
static
MipsRelocationFactory::Result abs32(Relocation& pReloc,
                                    const MCLDInfo& pLDInfo,
                                    MipsRelocationFactory& pParent)
{
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  RelocationFactory::DWord S = pReloc.symValue();

  pReloc.target() = S + A;
  return MipsRelocationFactory::OK;
}

// R_MIPS_HI16:
//   local/external: ((AHL + S) - (short)(AHL + S)) >> 16
//   _gp_disp      : ((AHL + GP - P) - (short)(AHL + GP - P)) >> 16
static
MipsRelocationFactory::Result hi16(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   MipsRelocationFactory& pParent)
{
  Relocation* lo_reloc = helper_FindLo16Reloc(pReloc);
  assert(NULL != lo_reloc && "There is no pair for R_MIPS_HI16");

  RelocationFactory::DWord AHI = pReloc.target() + pReloc.addend();
  RelocationFactory::DWord ALO = lo_reloc->target() + lo_reloc->addend();
  RelocationFactory::DWord AHL = (AHI & 0xFFFF) << 16 + (short)(ALO & 0xFFFF);
  RelocationFactory::DWord S = pReloc.symValue();

  if (helper_isGpDisp(pReloc)) {
    RelocationFactory::Address P = pReloc.place(pParent.getLayout());
    S = helper_GetGOTAddr(pParent) - P;
  }

  pReloc.target() &= 0xFFFF0000;
  pReloc.target() |= (((S + AHL + (int)0x8000) >> 16) & 0xFFFF);

  return MipsRelocationFactory::OK;
}

// R_MIPS_LO16:
//   local/external: AHL + S
//   _gp_disp      : AHL + GP - P + 4
static
MipsRelocationFactory::Result lo16(Relocation& pReloc,
                                   const MCLDInfo& pLDInfo,
                                   MipsRelocationFactory& pParent)
{
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  RelocationFactory::DWord S = pReloc.symValue();

  if (helper_isGpDisp(pReloc)) {
    RelocationFactory::Address P = pReloc.place(pParent.getLayout());
    S = helper_GetGOTAddr(pParent) - P;
  }

  pReloc.target() &= 0xFFFF0000;
  pReloc.target() |= ((S + A) & 0xFFFF);

  return MipsRelocationFactory::OK;
}

// R_MIPS_GOT16:
//   local   : tbd
//   external: G
static
MipsRelocationFactory::Result got16(Relocation& pReloc,
                                    const MCLDInfo& pLDInfo,
                                    MipsRelocationFactory& pParent)
{
  RelocationFactory::Address G = 0;

  ResolveInfo* rsym = pReloc.symInfo();
  if (rsym->isLocal()) {
    assert(0 && "R_MIPS_GOT16 relocation for a local sym is not implemented");
  }
  else {
    G = helper_GetGOTOffset(pReloc, pParent);
  }

  pReloc.target() &= 0xFFFF0000;
  pReloc.target() |= ((G - 0x7FF0) & 0xFFFF);

  return MipsRelocationFactory::OK;
}

// R_MIPS_CALL16: G
static
MipsRelocationFactory::Result call16(Relocation& pReloc,
                                     const MCLDInfo& pLDInfo,
                                     MipsRelocationFactory& pParent)
{
  RelocationFactory::Address G = helper_GetGOTOffset(pReloc, pParent);

  pReloc.target() &= 0xFFFF0000;
  pReloc.target() |= ((G - 0x7FF0) & 0xFFFF);

  return MipsRelocationFactory::OK;
}

// R_MIPS_GPREL32: A + S + GP0 - GP
static
MipsRelocationFactory::Result gprel32(Relocation& pReloc,
                                      const MCLDInfo& pLDInfo,
                                      MipsRelocationFactory& pParent)
{
  RelocationFactory::DWord A = pReloc.target() + pReloc.addend();
  RelocationFactory::DWord S = pReloc.symValue();
  RelocationFactory::Address GP = helper_GetGOTAddr(pParent);

  pReloc.target() = A + S - GP;
  return MipsRelocationFactory::OK;
}
