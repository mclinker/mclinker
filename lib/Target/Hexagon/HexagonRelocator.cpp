//===- HexagonRelocator.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <mcld/LD/LDSymbol.h>
#include <llvm/Support/DataTypes.h>
#include <llvm/Support/ELF.h>
#include <mcld/Support/MsgHandling.h>

#include "HexagonRelocator.h"
#include "HexagonRelocationFunctions.h"
#include "HexagonEncodings.h"

using namespace mcld;

//===--------------------------------------------------------------------===//
// Relocation Functions and Tables
//===--------------------------------------------------------------------===//
DECL_HEXAGON_APPLY_RELOC_FUNCS

    /// the prototype of applying function
    typedef Relocator::Result (*ApplyFunctionType)(Relocation &pReloc,
                                                   HexagonRelocator &pParent);

// the table entry of applying functions
struct ApplyFunctionTriple {
  ApplyFunctionType func;
  unsigned int type;
  const char *name;
};

// declare the table of applying functions
static const ApplyFunctionTriple ApplyFunctions[] = {
  DECL_HEXAGON_APPLY_RELOC_FUNC_PTRS
};

static uint32_t findBitMask(uint32_t insn, Instruction *encodings,
                            int32_t numInsns) {
  for (int32_t i = 0; i < numInsns; i++) {
    if (((insn & 0xc000) == 0) && !(encodings[i].isDuplex))
      continue;

    if (((insn & 0xc000) != 0) && (encodings[i].isDuplex))
      continue;

    if (((encodings[i].insnMask) & insn) == encodings[i].insnCmpMask)
      return encodings[i].insnBitMask;
  }
  assert(0);
}

#define FINDBITMASK(INSN)                                                      \
  findBitMask((uint32_t) INSN, insn_encodings,                                 \
              sizeof(insn_encodings) / sizeof(Instruction))

//===--------------------------------------------------------------------===//
// HexagonRelocator
//===--------------------------------------------------------------------===//
HexagonRelocator::HexagonRelocator(HexagonLDBackend &pParent,
                                   const LinkerConfig &pConfig)
    : Relocator(pConfig), m_Target(pParent) {}

HexagonRelocator::~HexagonRelocator() {}

Relocator::Result HexagonRelocator::applyRelocation(Relocation &pRelocation) {
  Relocation::Type type = pRelocation.type();

  if (type > 85) { // 86-255 relocs do not exists for Hexagon
    return Relocator::Unknown;
  }

  // apply the relocation
  return ApplyFunctions[type].func(pRelocation, *this);
}

const char *HexagonRelocator::getName(Relocation::Type pType) const {
  return ApplyFunctions[pType].name;
}

Relocator::Size HexagonRelocator::getSize(Relocation::Type pType) const {
  return 32;
}

void HexagonRelocator::scanRelocation(Relocation &pReloc, IRBuilder &pLinker,
                                      Module &pModule, LDSection &pSection) {
  if (LinkerConfig::Object == config().codeGenType())
    return;

  pReloc.updateAddend();
  // rsym - The relocation target symbol
  ResolveInfo *rsym = pReloc.symInfo();
  assert(NULL != rsym &&
         "ResolveInfo of relocation not set while scanRelocation");

  if (config().isCodeStatic())
    return;

  assert(NULL != pSection.getLink());
  if (0 == (pSection.getLink()->flag() & llvm::ELF::SHF_ALLOC))
    return;

  if (rsym->isLocal()) // rsym is local
    scanLocalReloc(pReloc, pLinker, pModule, pSection);
  else // rsym is external
    scanGlobalReloc(pReloc, pLinker, pModule, pSection);

  // check if we should issue undefined reference for the relocation target
  // symbol
  if (rsym->isUndef() && !rsym->isDyn() && !rsym->isWeak() && !rsym->isNull())
    fatal(diag::undefined_reference) << rsym->name();
}

void HexagonRelocator::addCopyReloc(ResolveInfo &pSym,
                                    HexagonLDBackend &pTarget) {
  Relocation &rel_entry = *pTarget.getRelaDyn().consumeEntry();
  rel_entry.setType(pTarget.getCopyRelType());
  assert(pSym.outSymbol()->hasFragRef());
  rel_entry.targetRef().assign(*pSym.outSymbol()->fragRef());
  rel_entry.setSymInfo(&pSym);
}

void HexagonRelocator::scanLocalReloc(Relocation &pReloc, IRBuilder &pBuilder,
                                      Module &pModule, LDSection &pSection) {
  // rsym - The relocation target symbol
  ResolveInfo *rsym = pReloc.symInfo();

  switch (pReloc.type()) {

  case llvm::ELF::R_HEX_LO16:
  case llvm::ELF::R_HEX_HI16:
  case llvm::ELF::R_HEX_16:
  case llvm::ELF::R_HEX_8:
  case llvm::ELF::R_HEX_32_6_X:
  case llvm::ELF::R_HEX_16_X:
  case llvm::ELF::R_HEX_12_X:
  case llvm::ELF::R_HEX_11_X:
  case llvm::ELF::R_HEX_10_X:
  case llvm::ELF::R_HEX_9_X:
  case llvm::ELF::R_HEX_8_X:
  case llvm::ELF::R_HEX_7_X:
  case llvm::ELF::R_HEX_6_X:
    assert(!(rsym->reserved() & ReserveRel) &&
           "Cannot apply this relocation for read only section");
    return;

  case llvm::ELF::R_HEX_32:
    // If buiding PIC object (shared library or PIC executable),
    // a dynamic relocations with RELATIVE type to this location is needed.
    // Reserve an entry in .rel.dyn
    if (config().isCodeIndep()) {
      getTarget().getRelaDyn().reserveEntry();
      // set Rel bit
      rsym->setReserved(rsym->reserved() | ReserveRel);
      getTarget().checkAndSetHasTextRel(*pSection.getLink());
    }
    return;

  default:
    return;
  }
}

void HexagonRelocator::scanGlobalReloc(Relocation &pReloc, IRBuilder &pBuilder,
                                       Module &pModule, LDSection &pSection) {
  // rsym - The relocation target symbol
  ResolveInfo *rsym = pReloc.symInfo();
  HexagonLDBackend &ld_backend = getTarget();

  switch (pReloc.type()) {
  case llvm::ELF::R_HEX_LO16:
  case llvm::ELF::R_HEX_HI16:
  case llvm::ELF::R_HEX_16:
  case llvm::ELF::R_HEX_8:
  case llvm::ELF::R_HEX_32_6_X:
  case llvm::ELF::R_HEX_16_X:
  case llvm::ELF::R_HEX_12_X:
  case llvm::ELF::R_HEX_11_X:
  case llvm::ELF::R_HEX_10_X:
  case llvm::ELF::R_HEX_9_X:
  case llvm::ELF::R_HEX_8_X:
  case llvm::ELF::R_HEX_7_X:
  case llvm::ELF::R_HEX_6_X:
    assert(!(rsym->reserved() & ReserveRel) &&
           "Cannot apply this relocation for read only section");
    return;

  case llvm::ELF::R_HEX_32:
    if (ld_backend.symbolNeedsPLT(*rsym)) {
      //create PLT for this symbol if it does not have.
      if (!(rsym->reserved() & ReservePLT)) {
        ld_backend.getPLT().reserveEntry();
        ld_backend.getGOTPLT().reserve();
        ld_backend.getRelaPLT().reserveEntry();
        rsym->setReserved(rsym->reserved() | ReservePLT);
      }
    }

    if (ld_backend.symbolNeedsDynRel(*rsym, (rsym->reserved() & ReservePLT),
                                     true)) {
      ld_backend.getRelaDyn().reserveEntry();
      if (ld_backend.symbolNeedsCopyReloc(pReloc, *rsym)) {
        LDSymbol &cpy_sym =
            defineSymbolforCopyReloc(pBuilder, *rsym, ld_backend);
        addCopyReloc(*cpy_sym.resolveInfo(), ld_backend);
      } else {
        rsym->setReserved(rsym->reserved() | ReserveRel);
        ld_backend.checkAndSetHasTextRel(*pSection.getLink());
      }
    }
    return;

  case llvm::ELF::R_HEX_GOTREL_LO16:
  case llvm::ELF::R_HEX_GOTREL_HI16:
  case llvm::ELF::R_HEX_GOTREL_32:
  case llvm::ELF::R_HEX_GOTREL_32_6_X:
  case llvm::ELF::R_HEX_GOTREL_16_X:
  case llvm::ELF::R_HEX_GOTREL_11_X:
    // This assumes that GOT exists
    return;

  case llvm::ELF::R_HEX_GOT_LO16:
  case llvm::ELF::R_HEX_GOT_HI16:
  case llvm::ELF::R_HEX_GOT_32:
  case llvm::ELF::R_HEX_GOT_16:
  case llvm::ELF::R_HEX_GOT_32_6_X:
  case llvm::ELF::R_HEX_GOT_16_X:
  case llvm::ELF::R_HEX_GOT_11_X:
    // Symbol needs GOT entry, reserve entry in .got
    // return if we already create GOT for this symbol
    if (rsym->reserved() & (ReserveGOT | GOTRel))
      return;
    ld_backend.getGOT().reserve();

    // If the GOT is used in statically linked binaries,
    // the GOT entry is enough and no relocation is needed.
    if (config().isCodeStatic()) {
      rsym->setReserved(rsym->reserved() | ReserveGOT);
      return;
    }
    // If building shared object or the symbol is undefined, a dynamic
    // relocation is needed to relocate this GOT entry. Reserve an
    // entry in .rel.dyn
    if (LinkerConfig::DynObj == config().codeGenType() || rsym->isUndef() ||
        rsym->isDyn()) {
      ld_backend.getRelaDyn().reserveEntry();
      // set GOTRel bit
      rsym->setReserved(rsym->reserved() | GOTRel);
      return;
    }
    // set GOT bit
    rsym->setReserved(rsym->reserved() | ReserveGOT);
    return;

  case llvm::ELF::R_HEX_B22_PCREL:
  case llvm::ELF::R_HEX_B15_PCREL:
  case llvm::ELF::R_HEX_B7_PCREL:
  case llvm::ELF::R_HEX_B13_PCREL:
  case llvm::ELF::R_HEX_B9_PCREL:
  case llvm::ELF::R_HEX_B32_PCREL_X:
  case llvm::ELF::R_HEX_B22_PCREL_X:
  case llvm::ELF::R_HEX_B15_PCREL_X:
  case llvm::ELF::R_HEX_B13_PCREL_X:
  case llvm::ELF::R_HEX_B9_PCREL_X:
  case llvm::ELF::R_HEX_B7_PCREL_X:
  case llvm::ELF::R_HEX_32_PCREL:
  case llvm::ELF::R_HEX_6_PCREL_X:
  case llvm::ELF::R_HEX_PLT_B22_PCREL:
    if (rsym->reserved() & ReservePLT)
      return;
    if (ld_backend.symbolNeedsPLT(*rsym) ||
        pReloc.type() == llvm::ELF::R_HEX_PLT_B22_PCREL) {
      ld_backend.getPLT().reserveEntry();
      ld_backend.getGOTPLT().reserve();
      ld_backend.getRelaPLT().reserveEntry();
      rsym->setReserved(rsym->reserved() | ReservePLT);
    }
    return;

  default:
    break;

  } // end of switch
}

/// defineSymbolforCopyReloc
/// For a symbol needing copy relocation, define a copy symbol in the BSS
/// section and all other reference to this symbol should refer to this
/// copy.
/// @note This is executed at `scan relocation' stage.
LDSymbol &HexagonRelocator::defineSymbolforCopyReloc(
    IRBuilder &pBuilder, const ResolveInfo &pSym, HexagonLDBackend &pTarget) {
  // get or create corresponding BSS LDSection
  LDSection *bss_sect_hdr = NULL;
  ELFFileFormat *file_format = pTarget.getOutputFormat();
  if (ResolveInfo::ThreadLocal == pSym.type())
    bss_sect_hdr = &file_format->getTBSS();
  else
    bss_sect_hdr = &file_format->getBSS();

  // get or create corresponding BSS SectionData
  assert(NULL != bss_sect_hdr);
  SectionData *bss_section = NULL;
  if (bss_sect_hdr->hasSectionData())
    bss_section = bss_sect_hdr->getSectionData();
  else
    bss_section = IRBuilder::CreateSectionData(*bss_sect_hdr);

  // Determine the alignment by the symbol value
  // FIXME: here we use the largest alignment
  uint32_t addralign = config().targets().bitclass() / 8;

  // allocate space in BSS for the copy symbol
  Fragment *frag = new FillFragment(0x0, 1, pSym.size());
  uint64_t size = ObjectBuilder::AppendFragment(*frag, *bss_section, addralign);
  bss_sect_hdr->setSize(bss_sect_hdr->size() + size);

  // change symbol binding to Global if it's a weak symbol
  ResolveInfo::Binding binding = (ResolveInfo::Binding) pSym.binding();
  if (binding == ResolveInfo::Weak)
    binding = ResolveInfo::Global;

  // Define the copy symbol in the bss section and resolve it
  LDSymbol *cpy_sym = pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
      pSym.name(), (ResolveInfo::Type) pSym.type(), ResolveInfo::Define,
      binding, pSym.size(), // size
      0x0,                  // value
      FragmentRef::Create(*frag, 0x0), (ResolveInfo::Visibility) pSym.other());

  // output all other alias symbols if any
  Module &pModule = pBuilder.getModule();
  Module::AliasList *alias_list = pModule.getAliasList(pSym);
  if (NULL != alias_list) {
    Module::alias_iterator it, it_e = alias_list->end();
    for (it = alias_list->begin(); it != it_e; ++it) {
      const ResolveInfo *alias = *it;
      if (alias != &pSym && alias->isDyn()) {
        pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
            alias->name(), (ResolveInfo::Type) alias->type(),
            ResolveInfo::Define, binding, alias->size(), // size
            0x0,                                         // value
            FragmentRef::Create(*frag, 0x0),
            (ResolveInfo::Visibility) alias->other());
      }
    }
  }

  return *cpy_sym;
}

void HexagonRelocator::partialScanRelocation(Relocation &pReloc,
                                             Module &pModule,
                                             const LDSection &pSection) {
  pReloc.updateAddend();
  // if we meet a section symbol
  if (pReloc.symInfo()->type() == ResolveInfo::Section) {
    LDSymbol *input_sym = pReloc.symInfo()->outSymbol();

    // 1. update the relocation target offset
    assert(input_sym->hasFragRef());
    // 2. get the output LDSection which the symbol defined in
    const LDSection &out_sect =
        input_sym->fragRef()->frag()->getParent()->getSection();
    ResolveInfo *sym_info =
        pModule.getSectionSymbolSet().get(out_sect)->resolveInfo();
    // set relocation target symbol to the output section symbol's resolveInfo
    pReloc.setSymInfo(sym_info);
  }
}

/// helper_DynRel - Get an relocation entry in .rela.dyn
static Relocation &helper_DynRel(ResolveInfo *pSym, Fragment &pFrag,
                                 uint64_t pOffset, HexagonRelocator::Type pType,
                                 HexagonRelocator &pParent) {
  HexagonLDBackend &ld_backend = pParent.getTarget();
  Relocation &rela_entry = *ld_backend.getRelaDyn().consumeEntry();
  rela_entry.setType(pType);
  rela_entry.targetRef().assign(pFrag, pOffset);
  if (pType == llvm::ELF::R_HEX_RELATIVE || NULL == pSym)
    rela_entry.setSymInfo(0);
  else
    rela_entry.setSymInfo(pSym);

  return rela_entry;
}

/// helper_use_relative_reloc - Check if symbol can use relocation
/// R_HEX_RELATIVE
static bool helper_use_relative_reloc(const ResolveInfo &pSym,
                                      const HexagonRelocator &pFactory) {
  // if symbol is dynamic or undefine or preemptible
  if (pSym.isDyn() || pSym.isUndef() ||
      pFactory.getTarget().isSymbolPreemptible(pSym))
    return false;
  return true;
}

static HexagonGOTEntry &helper_get_GOT_and_init(Relocation &pReloc,
                                                HexagonRelocator &pParent) {
  // rsym - The relocation target symbol
  ResolveInfo *rsym = pReloc.symInfo();
  HexagonLDBackend &ld_backend = pParent.getTarget();

  HexagonGOTEntry *got_entry = pParent.getSymGOTMap().lookUp(*rsym);
  if (NULL != got_entry)
    return *got_entry;

  // not found
  got_entry = ld_backend.getGOT().consume();
  pParent.getSymGOTMap().record(*rsym, *got_entry);

  // If we first get this GOT entry, we should initialize it.
  if (rsym->reserved() & HexagonRelocator::ReserveGOT) {
    // No corresponding dynamic relocation, initialize to the symbol value.
    got_entry->setValue(pReloc.symValue());
  } else if (rsym->reserved() & HexagonRelocator::GOTRel) {
    // Initialize got_entry content and the corresponding dynamic relocation.
    if (helper_use_relative_reloc(*rsym, pParent)) {
      helper_DynRel(rsym, *got_entry, 0x0, llvm::ELF::R_HEX_RELATIVE, pParent);
      got_entry->setValue(pReloc.symValue());
    } else {
      helper_DynRel(rsym, *got_entry, 0x0, llvm::ELF::R_HEX_GLOB_DAT, pParent);
      got_entry->setValue(0);
    }
  } else {
    fatal(diag::reserve_entry_number_mismatch_got);
  }
  return *got_entry;
}

static HexagonRelocator::Address helper_GOT_ORG(HexagonRelocator &pParent) {
  return pParent.getTarget().getGOT().addr();
}

static HexagonRelocator::Address helper_GOT(Relocation &pReloc,
                                            HexagonRelocator &pParent) {
  HexagonGOTEntry &got_entry = helper_get_GOT_and_init(pReloc, pParent);
  return helper_GOT_ORG(pParent) + got_entry.getOffset();
}

static PLTEntryBase &helper_get_PLT_and_init(Relocation &pReloc,
                                             HexagonRelocator &pParent) {
  // rsym - The relocation target symbol
  ResolveInfo *rsym = pReloc.symInfo();
  HexagonLDBackend &ld_backend = pParent.getTarget();

  PLTEntryBase *plt_entry = pParent.getSymPLTMap().lookUp(*rsym);
  if (NULL != plt_entry)
    return *plt_entry;

  // not found
  plt_entry = ld_backend.getPLT().consume();
  pParent.getSymPLTMap().record(*rsym, *plt_entry);
  // If we first get this PLT entry, we should initialize it.
  if (rsym->reserved() & HexagonRelocator::ReservePLT) {
    HexagonGOTEntry *gotplt_entry = pParent.getSymGOTPLTMap().lookUp(*rsym);
    assert(NULL == gotplt_entry &&
           "PLT entry not exist, but DynRel entry exist!");
    gotplt_entry = ld_backend.getGOTPLT().consume();
    pParent.getSymGOTPLTMap().record(*rsym, *gotplt_entry);
    // init the corresponding rel entry in .rel.plt
    Relocation &rela_entry = *ld_backend.getRelaPLT().consumeEntry();
    rela_entry.setType(llvm::ELF::R_HEX_JMP_SLOT);
    rela_entry.targetRef().assign(*gotplt_entry);
    rela_entry.setSymInfo(rsym);
  } else {
    fatal(diag::reserve_entry_number_mismatch_plt);
  }

  return *plt_entry;
}

static HexagonRelocator::Address helper_PLT_ORG(HexagonRelocator &pParent) {
  return pParent.getTarget().getPLT().addr();
}

static HexagonRelocator::Address helper_PLT(Relocation &pReloc,
                                            HexagonRelocator &pParent) {
  PLTEntryBase &plt_entry = helper_get_PLT_and_init(pReloc, pParent);
  return helper_PLT_ORG(pParent) + plt_entry.getOffset();
}

//=========================================//
// Each relocation function implementation //
//=========================================//

// R_HEX_NONE
HexagonRelocator::Result none(Relocation &pReloc, HexagonRelocator &pParent) {
  return HexagonRelocator::OK;
}

//R_HEX_32 and its class of relocations use only addend and symbol value
// S + A : result is unsigned truncate.
// Exception: R_HEX_32_6_X : unsigned verify
HexagonRelocator::Result applyAbs(Relocation &pReloc) {
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord A = pReloc.addend();
  uint32_t result = (uint32_t)(S + A);
  uint32_t bitMask = 0;
  uint32_t effectiveBits = 0;
  uint32_t alignment = 1;
  uint32_t shift = 0;

  switch (pReloc.type()) {
  case llvm::ELF::R_HEX_LO16:
    bitMask = 0x00c03fff;
    break;

  case llvm::ELF::R_HEX_HI16:
    shift = 16;
    bitMask = 0x00c03fff;
    break;

  case llvm::ELF::R_HEX_32:
    bitMask = 0xffffffff;
    break;

  case llvm::ELF::R_HEX_16:
    bitMask = 0x0000ffff;
    alignment = 2;
    break;

  case llvm::ELF::R_HEX_8:
    bitMask = 0x000000ff;
    alignment = 1;
    break;

  case llvm::ELF::R_HEX_12_X:
    bitMask = 0x000007e0;
    break;

  case llvm::ELF::R_HEX_32_6_X:
    bitMask = 0xfff3fff;
    shift = 6;
    effectiveBits = 26;
    break;

  case llvm::ELF::R_HEX_16_X:
  case llvm::ELF::R_HEX_11_X:
  case llvm::ELF::R_HEX_10_X:
  case llvm::ELF::R_HEX_9_X:
  case llvm::ELF::R_HEX_8_X:
  case llvm::ELF::R_HEX_7_X:
  case llvm::ELF::R_HEX_6_X:
    bitMask = FINDBITMASK(pReloc.target());
    break;

  default:
    // show proper error
    fatal(diag::unsupported_relocation) << (int)
        pReloc.type() << "mclinker@googlegroups.com";

  }

  if ((shift != 0) && (result % alignment != 0))
    return HexagonRelocator::BadReloc;

  result >>= shift;

  if (effectiveBits) {
    uint32_t range = 1 << effectiveBits;
    if (result > (range - 1))
      return HexagonRelocator::Overflow;
  }

  pReloc.target() |= ApplyMask<uint32_t>(bitMask, result);
  return HexagonRelocator::OK;
}

//R_HEX_B22_PCREL and its class of relocations, use
// S + A - P : result is signed verify.
// Exception: R_HEX_B32_PCREL_X : signed truncate
// Another Exception: R_HEX_6_PCREL_X is unsigned truncate
HexagonRelocator::Result applyRel(Relocation &pReloc, int64_t pResult) {
  uint32_t bitMask = 0;
  uint32_t effectiveBits = 0;
  uint32_t alignment = 1;
  uint32_t result;
  uint32_t shift = 0;

  switch (pReloc.type()) {
  case llvm::ELF::R_HEX_B22_PCREL:
    bitMask = 0x01ff3ffe;
    effectiveBits = 22;
    alignment = 4;
    shift = 2;
    break;

  case llvm::ELF::R_HEX_B15_PCREL:
    bitMask = 0x00df20fe;
    effectiveBits = 15;
    alignment = 4;
    shift = 2;
    break;

  case llvm::ELF::R_HEX_B7_PCREL:
    bitMask = 0x00001f18;
    effectiveBits = 7;
    alignment = 4;
    shift = 2;
    break;

  case llvm::ELF::R_HEX_B13_PCREL:
    bitMask = 0x00202ffe;
    effectiveBits = 13;
    alignment = 4;
    shift = 2;
    break;

  case llvm::ELF::R_HEX_B9_PCREL:
    bitMask = 0x003000fe;
    effectiveBits = 9;
    alignment = 4;
    shift = 2;
    break;

  case llvm::ELF::R_HEX_B32_PCREL_X:
    bitMask = 0xfff3fff;
    shift = 6;
    break;

  case llvm::ELF::R_HEX_B22_PCREL_X:
    bitMask = 0x01ff3ffe;
    effectiveBits = 22;
    pResult &= 0x3f;
    break;

  case llvm::ELF::R_HEX_B15_PCREL_X:
    bitMask = 0x00df20fe;
    effectiveBits = 15;
    pResult &= 0x3f;
    break;

  case llvm::ELF::R_HEX_B13_PCREL_X:
    bitMask = 0x00202ffe;
    effectiveBits = 13;
    pResult &= 0x3f;
    break;

  case llvm::ELF::R_HEX_B9_PCREL_X:
    bitMask = 0x003000fe;
    effectiveBits = 9;
    pResult &= 0x3f;
    break;

  case llvm::ELF::R_HEX_B7_PCREL_X:
    bitMask = 0x00001f18;
    effectiveBits = 7;
    pResult &= 0x3f;
    break;

  case llvm::ELF::R_HEX_32_PCREL:
    bitMask = 0xffffffff;
    effectiveBits = 32;
    break;

  case llvm::ELF::R_HEX_6_PCREL_X:
    // This is unique since it has a unsigned operand and its truncated
    bitMask = FINDBITMASK(pReloc.target());
    result = pReloc.addend() + pReloc.symValue() - pReloc.place();
    pReloc.target() |= ApplyMask<uint32_t>(bitMask, result);
    return HexagonRelocator::OK;

  default:
    // show proper error
    fatal(diag::unsupported_relocation) << (int)
        pReloc.type() << "mclinker@googlegroups.com";
  }

  if ((shift != 0) && (pResult % alignment != 0))
    return HexagonRelocator::BadReloc;

  pResult >>= shift;

  if (effectiveBits) {
    int64_t range = 1LL << (effectiveBits - 1);
    if ((pResult > (range - 1)) || (pResult < -range))
      return HexagonRelocator::Overflow;
  }

  pReloc.target() |= (uint32_t) ApplyMask<int32_t>(bitMask, pResult);
  return HexagonRelocator::OK;
}

HexagonRelocator::Result relocAbs(Relocation &pReloc,
                                  HexagonRelocator &pParent) {
  ResolveInfo *rsym = pReloc.symInfo();
  HexagonLDBackend &ld_backend = pParent.getTarget();
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord A = pReloc.addend();

  bool has_dyn_rel = ld_backend.symbolNeedsDynRel(
      *rsym, (rsym->reserved() & HexagonRelocator::ReservePLT), true);

  FragmentRef &target_fragref = pReloc.targetRef();
  Fragment *target_frag = target_fragref.frag();

  LDSection &target_sect = target_frag->getParent()->getSection();
  // if the flag of target section is not ALLOC, we eprform only static
  // relocation.
  if (0 == (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    return applyAbs(pReloc);
  }

  // a local symbol with .rela type relocation
  if (rsym->isLocal() && has_dyn_rel) {
    HexagonRelocator::Type type = pReloc.type();
    if (llvm::ELF::R_HEX_32 == type)
      type = llvm::ELF::R_HEX_RELATIVE;
    Relocation &rel_entry = helper_DynRel(
        rsym, *target_frag, target_fragref.offset(), type, pParent);
    rel_entry.setAddend(S + A);
    return HexagonRelocator::OK;
  }

  if (!rsym->isLocal()) {
    if (rsym->reserved() & HexagonRelocator::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
    }

    if (has_dyn_rel) {
      if (llvm::ELF::R_HEX_32 == pReloc.type() &&
          helper_use_relative_reloc(*rsym, pParent)) {
        Relocation &rel_entry =
            helper_DynRel(rsym, *target_frag, target_fragref.offset(),
                          llvm::ELF::R_HEX_RELATIVE, pParent);
        rel_entry.setAddend(S + A);
      } else {
        Relocation &rel_entry =
            helper_DynRel(rsym, *target_frag, target_fragref.offset(),
                          pReloc.type(), pParent);
        rel_entry.setAddend(A);
        return HexagonRelocator::OK;
      }
    }
  }

  return applyAbs(pReloc);
}

HexagonRelocator::Result relocPCREL(Relocation &pReloc,
                                    HexagonRelocator &pParent) {
  ResolveInfo *rsym = pReloc.symInfo();
  int64_t result;

  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord A = pReloc.addend();
  HexagonRelocator::DWord P = pReloc.place();

  FragmentRef &target_fragref = pReloc.targetRef();
  Fragment *target_frag = target_fragref.frag();
  LDSection &target_sect = target_frag->getParent()->getSection();

  result = (int64_t)(S + A - P);

  // for relocs inside non ALLOC, just apply
  if (0 == (llvm::ELF::SHF_ALLOC & target_sect.flag())) {
    return applyRel(pReloc, result);
  }

  if (!rsym->isLocal()) {
    if (rsym->reserved() & HexagonRelocator::ReservePLT) {
      S = helper_PLT(pReloc, pParent);
      result = (int64_t)(S + A - P);
      applyRel(pReloc, result);
      return HexagonRelocator::OK;
    }
  }

  return applyRel(pReloc, result);
}

// R_HEX_GPREL16_0 and its class : Unsigned Verify
HexagonRelocator::Result relocGPREL(Relocation &pReloc,
                                    HexagonRelocator &pParent) {
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord A = pReloc.addend();
  HexagonRelocator::DWord GP = pParent.getTarget().getGP();

  uint32_t result = (uint32_t)(S + A - GP);
  uint32_t shift = 0;
  uint32_t alignment = 1;

  switch (pReloc.type()) {
  case llvm::ELF::R_HEX_GPREL16_0:
    break;

  case llvm::ELF::R_HEX_GPREL16_1:
    shift = 1;
    alignment = 2;
    break;

  case llvm::ELF::R_HEX_GPREL16_2:
    shift = 2;
    alignment = 4;
    break;

  case llvm::ELF::R_HEX_GPREL16_3:
    shift = 3;
    alignment = 8;
    break;

  default:
    // show proper error
    fatal(diag::unsupported_relocation) << (int)
        pReloc.type() << "mclinker@googlegroups.com";
  }

  uint32_t range = 1 << 16;
  uint32_t bitMask = FINDBITMASK(pReloc.target());

  if ((shift != 0) && (result % alignment != 0))
    return HexagonRelocator::BadReloc;

  result >>= shift;

  if (result < range - 1) {
    pReloc.target() |= ApplyMask<uint32_t>(bitMask, result);
    return HexagonRelocator::OK;
  }
  return HexagonRelocator::Overflow;
}

// R_HEX_PLT_B22_PCREL: PLT(S) + A - P
HexagonRelocator::Result relocPLTB22PCREL(Relocation &pReloc,
                                          HexagonRelocator &pParent) {
  // PLT_S depends on if there is a PLT entry.
  HexagonRelocator::Address PLT_S;
  if ((pReloc.symInfo()->reserved() & HexagonRelocator::ReservePLT))
    PLT_S = helper_PLT(pReloc, pParent);
  else
    PLT_S = pReloc.symValue();
  HexagonRelocator::Address P = pReloc.place();
  uint32_t bitMask = FINDBITMASK(pReloc.target());
  uint32_t result = (PLT_S + pReloc.addend() - P) >> 2;
  pReloc.target() = pReloc.target() | ApplyMask<uint32_t>(bitMask, result);
  return HexagonRelocator::OK;
}

//R_HEX_GOT_LO16 and its class : (G) Signed Truncate
//Exception: R_HEX_GOT_16(_X): signed verify
// Exception: R_HEX_GOT_11_X : unsigned truncate
HexagonRelocator::Result relocGOT(Relocation &pReloc,
                                  HexagonRelocator &pParent) {
  if (!(pReloc.symInfo()->reserved() &
        (HexagonRelocator::ReserveGOT | HexagonRelocator::GOTRel))) {
    return HexagonRelocator::BadReloc;
  }

  HexagonRelocator::Address GOT_S = helper_GOT(pReloc, pParent);
  HexagonRelocator::Address GOT = pParent.getTarget().getGOTSymbolAddr();
  int32_t result = (int32_t)(GOT_S - GOT);
  uint32_t effectiveBits = 0;
  uint32_t alignment = 1;
  uint32_t bitMask = 0;
  uint32_t result_u;
  uint32_t shift = 0;

  switch (pReloc.type()) {
  case llvm::ELF::R_HEX_GOT_LO16:
    bitMask = 0x00c03fff;
    break;

  case llvm::ELF::R_HEX_GOT_HI16:
    bitMask = 0x00c03fff;
    shift = 16;
    alignment = 4;
    break;

  case llvm::ELF::R_HEX_GOT_32:
    bitMask = 0xffffffff;
    break;

  case llvm::ELF::R_HEX_GOT_16:
    bitMask = FINDBITMASK(pReloc.target());
    effectiveBits = 16;
    break;

  case llvm::ELF::R_HEX_GOT_32_6_X:
    bitMask = 0xfff3fff;
    shift = 6;
    break;

  case llvm::ELF::R_HEX_GOT_16_X:
    bitMask = FINDBITMASK(pReloc.target());
    effectiveBits = 6;
    break;

  case llvm::ELF::R_HEX_GOT_11_X:
    bitMask = FINDBITMASK(pReloc.target());
    result_u = GOT_S - GOT;
    pReloc.target() |= ApplyMask<uint32_t>(bitMask, result_u);
    return HexagonRelocator::OK;

  default:
    // show proper error
    fatal(diag::unsupported_relocation) << (int)
        pReloc.type() << "mclinker@googlegroups.com";
  }

  if ((shift != 0) && (result % alignment != 0))
    return HexagonRelocator::BadReloc;

  result >>= shift;

  if (effectiveBits) {
    int32_t range = 1 << (effectiveBits - 1);
    if ((result > range - 1) || (result < -range))
      return HexagonRelocator::Overflow;
  }
  pReloc.target() |= ApplyMask<int32_t>(bitMask, result);
  return HexagonRelocator::OK;
}

// R_HEX_GOTREL_LO16: and its class of relocs
// (S + A - GOT) : Signed Truncate
HexagonRelocator::Result relocGOTREL(Relocation &pReloc,
                                     HexagonRelocator &pParent) {
  HexagonRelocator::Address S = pReloc.symValue();
  HexagonRelocator::DWord A = pReloc.addend();
  HexagonRelocator::Address GOT = pParent.getTarget().getGOTSymbolAddr();

  uint32_t bitMask = 0;
  uint32_t alignment = 1;
  uint32_t shift = 0;

  uint32_t result = (uint32_t)(S + A - GOT);

  switch (pReloc.type()) {
  case llvm::ELF::R_HEX_GOTREL_LO16:
    bitMask = 0x00c03fff;
    break;

  case llvm::ELF::R_HEX_GOTREL_HI16:
    bitMask = 0x00c03fff;
    shift = 16;
    alignment = 4;
    break;

  case llvm::ELF::R_HEX_GOTREL_32:
    bitMask = 0xffffffff;
    break;

  case llvm::ELF::R_HEX_GOTREL_32_6_X:
    bitMask = 0x0fff3fff;
    shift = 6;
    break;

  case llvm::ELF::R_HEX_GOTREL_16_X:
  case llvm::ELF::R_HEX_GOTREL_11_X:
    bitMask = FINDBITMASK(pReloc.target());
    break;

  default:
    // show proper error
    fatal(diag::unsupported_relocation) << (int)
        pReloc.type() << "mclinker@googlegroups.com";
  }

  if (result % alignment != 0)
    return HexagonRelocator::BadReloc;

  result >>= shift;

  pReloc.target() |= ApplyMask<uint32_t>(bitMask, result);
  return HexagonRelocator::OK;
}

HexagonRelocator::Result unsupport(Relocation &pReloc,
                                   HexagonRelocator &pParent) {
  return HexagonRelocator::Unsupport;
}
