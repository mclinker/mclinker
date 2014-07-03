//===- MipsLDBackend.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "Mips.h"
#include "MipsGNUInfo.h"
#include "MipsELFDynamic.h"
#include "MipsLA25Stub.h"
#include "MipsLDBackend.h"
#include "MipsRelocator.h"

#include <llvm/ADT/Triple.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/Host.h>

#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/IRBuilder.h>
#include <mcld/LD/BranchIslandFactory.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/StubFactory.h>
#include <mcld/LD/ELFFileFormat.h>
#include <mcld/MC/Attribute.h>
#include <mcld/Fragment/FillFragment.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Target/OutputRelocSection.h>
#include <mcld/Object/ObjectBuilder.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// MipsGNULDBackend
//===----------------------------------------------------------------------===//
MipsGNULDBackend::MipsGNULDBackend(const LinkerConfig& pConfig,
                                   MipsGNUInfo* pInfo)
  : GNULDBackend(pConfig, pInfo),
    m_pRelocator(NULL),
    m_pGOT(NULL),
    m_pPLT(NULL),
    m_pGOTPLT(NULL),
    m_pInfo(*pInfo),
    m_pRelPlt(NULL),
    m_pRelDyn(NULL),
    m_pDynamic(NULL),
    m_pGOTSymbol(NULL),
    m_pPLTSymbol(NULL),
    m_pGpDispSymbol(NULL)
{
}

MipsGNULDBackend::~MipsGNULDBackend()
{
  delete m_pRelocator;
  delete m_pPLT;
  delete m_pRelPlt;
  delete m_pRelDyn;
  delete m_pDynamic;
}

bool MipsGNULDBackend::needsLA25Stub(Relocation::Type pType,
                                     const mcld::ResolveInfo* pSym)
{
  if (config().isCodeIndep())
    return false;

  if (llvm::ELF::R_MIPS_26 != pType)
    return false;

  if (pSym->isLocal())
    return false;

  return true;
}

void MipsGNULDBackend::addNonPICBranchSym(ResolveInfo* rsym)
{
  m_HasNonPICBranchSyms.insert(rsym);
}

bool MipsGNULDBackend::hasNonPICBranch(const ResolveInfo* rsym) const
{
  return m_HasNonPICBranchSyms.count(rsym);
}

void MipsGNULDBackend::initTargetSections(Module& pModule,
                                            ObjectBuilder& pBuilder)
{
  if (LinkerConfig::Object == config().codeGenType())
    return;

  ELFFileFormat* file_format = getOutputFormat();

  // initialize .rel.plt
  LDSection& relplt = file_format->getRelPlt();
  m_pRelPlt = new OutputRelocSection(pModule, relplt);

  // initialize .rel.dyn
  LDSection& reldyn = file_format->getRelDyn();
  m_pRelDyn = new OutputRelocSection(pModule, reldyn);
}

void MipsGNULDBackend::initTargetSymbols(IRBuilder& pBuilder, Module& pModule)
{
  // Define the symbol _GLOBAL_OFFSET_TABLE_ if there is a symbol with the
  // same name in input
  m_pGOTSymbol = pBuilder.AddSymbol<IRBuilder::AsReferred, IRBuilder::Resolve>(
                   "_GLOBAL_OFFSET_TABLE_",
                   ResolveInfo::Object,
                   ResolveInfo::Define,
                   ResolveInfo::Local,
                   0x0,  // size
                   0x0,  // value
                   FragmentRef::Null(), // FragRef
                   ResolveInfo::Hidden);

  // Define the symbol _PROCEDURE_LINKAGE_TABLE_ if there is a symbol with the
  // same name in input
  m_pPLTSymbol =
    pBuilder.AddSymbol<IRBuilder::AsReferred, IRBuilder::Resolve>(
                   "_PROCEDURE_LINKAGE_TABLE_",
                   ResolveInfo::Object,
                   ResolveInfo::Define,
                   ResolveInfo::Local,
                   0x0,  // size
                   0x0,  // value
                   FragmentRef::Null(), // FragRef
                   ResolveInfo::Hidden);

  m_pGpDispSymbol = pBuilder.AddSymbol<IRBuilder::AsReferred, IRBuilder::Resolve>(
                   "_gp_disp",
                   ResolveInfo::Section,
                   ResolveInfo::Define,
                   ResolveInfo::Absolute,
                   0x0,  // size
                   0x0,  // value
                   FragmentRef::Null(), // FragRef
                   ResolveInfo::Default);
  pBuilder.AddSymbol<IRBuilder::AsReferred, IRBuilder::Unresolve>(
                   "_gp",
                   ResolveInfo::NoType,
                   ResolveInfo::Define,
                   ResolveInfo::Absolute,
                   0x0,  // size
                   0x0,  // value
                   FragmentRef::Null(), // FragRef
                   ResolveInfo::Default);
}

const Relocator* MipsGNULDBackend::getRelocator() const
{
  assert(NULL != m_pRelocator);
  return m_pRelocator;
}

Relocator* MipsGNULDBackend::getRelocator()
{
  assert(NULL != m_pRelocator);
  return m_pRelocator;
}

void MipsGNULDBackend::doPreLayout(IRBuilder& pBuilder)
{
  // initialize .dynamic data
  if (!config().isCodeStatic() && NULL == m_pDynamic)
    m_pDynamic = new MipsELFDynamic(*this, config());

  // set .got size
  // when building shared object, the .got section is must.
  if (LinkerConfig::Object != config().codeGenType()) {
    if (LinkerConfig::DynObj == config().codeGenType() ||
        m_pGOT->hasGOT1() ||
        NULL != m_pGOTSymbol) {
      m_pGOT->finalizeScanning(*m_pRelDyn);
      m_pGOT->finalizeSectionSize();

      defineGOTSymbol(pBuilder);
    }

    if (m_pGOTPLT->hasGOT1()) {
      m_pGOTPLT->finalizeSectionSize();

      defineGOTPLTSymbol(pBuilder);
    }

    if (m_pPLT->hasPLT1())
      m_pPLT->finalizeSectionSize();

    ELFFileFormat* file_format = getOutputFormat();

    // set .rel.plt size
    if (!m_pRelPlt->empty()) {
      assert(!config().isCodeStatic() &&
            "static linkage should not result in a dynamic relocation section");
      file_format->getRelPlt().setSize(
                                  m_pRelPlt->numOfRelocs() * getRelEntrySize());
    }

    // set .rel.dyn size
    if (!m_pRelDyn->empty()) {
      assert(!config().isCodeStatic() &&
            "static linkage should not result in a dynamic relocation section");
      file_format->getRelDyn().setSize(
                                  m_pRelDyn->numOfRelocs() * getRelEntrySize());
    }
  }
}

void MipsGNULDBackend::doPostLayout(Module& pModule, IRBuilder& pBuilder)
{
  const ELFFileFormat *format = getOutputFormat();

  if (format->hasGOTPLT()) {
    assert(m_pGOTPLT && "doPostLayout failed, m_pGOTPLT is NULL!");
    m_pGOTPLT->applyAllGOTPLT(m_pPLT->addr());
  }

  if (format->hasPLT()) {
    assert(m_pPLT && "doPostLayout failed, m_pPLT is NULL!");
    m_pPLT->applyAllPLT(*m_pGOTPLT);
  }

  m_pInfo.setABIVersion(m_pPLT && m_pPLT->hasPLT1() ? 1 : 0);

  // FIXME: (simon) We need to iterate all input sections
  // check that flags are consistent and merge them properly.
  uint64_t picFlags = llvm::ELF::EF_MIPS_CPIC;
  if (config().targets().triple().isArch64Bit()) {
    picFlags |= llvm::ELF::EF_MIPS_PIC;
  }
  else {
    if (LinkerConfig::DynObj == config().codeGenType())
      picFlags |= llvm::ELF::EF_MIPS_PIC;
  }

  m_pInfo.setPICFlags(picFlags);
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
MipsELFDynamic& MipsGNULDBackend::dynamic()
{
  assert(NULL != m_pDynamic);
  return *m_pDynamic;
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
const MipsELFDynamic& MipsGNULDBackend::dynamic() const
{
  assert(NULL != m_pDynamic);
  return *m_pDynamic;
}

uint64_t MipsGNULDBackend::emitSectionData(const LDSection& pSection,
                                           MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  const ELFFileFormat* file_format = getOutputFormat();

  if (file_format->hasGOT() && (&pSection == &(file_format->getGOT()))) {
    return m_pGOT->emit(pRegion);
  }

  if (file_format->hasPLT() && (&pSection == &(file_format->getPLT()))) {
    return m_pPLT->emit(pRegion);
  }

  if (file_format->hasGOTPLT() && (&pSection == &(file_format->getGOTPLT()))) {
    return m_pGOTPLT->emit(pRegion);
  }

  fatal(diag::unrecognized_output_sectoin)
          << pSection.name()
          << "mclinker@googlegroups.com";
  return 0;
}

bool MipsGNULDBackend::hasEntryInStrTab(const LDSymbol& pSym) const
{
  return ResolveInfo::Section != pSym.type() ||
         m_pGpDispSymbol == &pSym;
}

namespace {
  struct DynsymGOTCompare
  {
    const MipsGOT& m_pGOT;

    DynsymGOTCompare(const MipsGOT& pGOT)
      : m_pGOT(pGOT)
    {
    }

    bool operator()(const LDSymbol* X, const LDSymbol* Y) const
    {
      return m_pGOT.dynSymOrderCompare(X, Y);
    }
  };
}

void MipsGNULDBackend::orderSymbolTable(Module& pModule)
{
  if (GeneralOptions::GNU  == config().options().getHashStyle() ||
      GeneralOptions::Both == config().options().getHashStyle()) {
    // The MIPS ABI and .gnu.hash require .dynsym to be sorted
    // in different ways. The MIPS ABI requires a mapping between
    // the GOT and the symbol table. At the same time .gnu.hash
    // needs symbols to be grouped by hash code.
    llvm::errs() << ".gnu.hash is incompatible with the MIPS ABI\n";
  }

  Module::SymbolTable& symbols = pModule.getSymbolTable();

  std::stable_sort(symbols.dynamicBegin(), symbols.dynamicEnd(),
                   DynsymGOTCompare(*m_pGOT));
}

namespace llvm {
namespace ELF {
// SHT_MIPS_OPTIONS section's block descriptor.
struct Elf_Options {
  unsigned char kind;     // Determines interpretation of variable
                          // part of descriptor. See ODK_xxx enumeration.
  unsigned char size;     // Byte size of descriptor, including this header.
  Elf64_Half    section;  // Section header index of section affected,
                          // or 0 for global options.
  Elf64_Word    info;     // Kind-speciﬁc information.
};

// Type of SHT_MIPS_OPTIONS section's block.
enum {
  ODK_NULL       = 0, // Undefined.
  ODK_REGINFO    = 1, // Register usage and GP value.
  ODK_EXCEPTIONS = 2, // Exception processing information.
  ODK_PAD        = 3, // Section padding information.
  ODK_HWPATCH    = 4, // Hardware workarounds performed.
  ODK_FILL       = 5, // Fill value used by the linker.
  ODK_TAGS       = 6, // Reserved space for desktop tools.
  ODK_HWAND      = 7, // Hardware workarounds, AND bits when merging.
  ODK_HWOR       = 8, // Hardware workarounds, OR bits when merging.
  ODK_GP_GROUP   = 9, // GP group to use for text/data sections.
  ODK_IDENT      = 10 // ID information.
};

// Content of ODK_REGINFO block in SHT_MIPS_OPTIONS section on 32 bit ABI.
struct Elf32_RegInfo {
  Elf32_Word ri_gprmask;    // Mask of general purpose registers used.
  Elf32_Word ri_cprmask[4]; // Mask of co-processor registers used.
  Elf32_Addr ri_gp_value;   // GP register value for this object file.
};

// Content of ODK_REGINFO block in SHT_MIPS_OPTIONS section on 64 bit ABI.
struct Elf64_RegInfo {
  Elf32_Word ri_gprmask;    // Mask of general purpose registers used.
  Elf32_Word ri_pad;        // Padding.
  Elf32_Word ri_cprmask[4]; // Mask of co-processor registers used.
  Elf64_Addr ri_gp_value;   // GP register value for this object file.
};

}
}

bool MipsGNULDBackend::readSection(Input& pInput, SectionData& pSD)
{
  llvm::StringRef name(pSD.getSection().name());

  if (name.startswith(".sdata")) {
    uint64_t offset = pInput.fileOffset() + pSD.getSection().offset();
    uint64_t size = pSD.getSection().size();

    Fragment* frag = IRBuilder::CreateRegion(pInput, offset, size);
    ObjectBuilder::AppendFragment(*frag, pSD);
    return true;
  }

  if (pSD.getSection().type() == llvm::ELF::SHT_MIPS_OPTIONS) {
    uint32_t offset = pInput.fileOffset() + pSD.getSection().offset();
    uint32_t size = pSD.getSection().size();

    llvm::StringRef region = pInput.memArea()->request(offset, size);
    if (region.size() > 0) {
      const llvm::ELF::Elf_Options* optb =
        reinterpret_cast<const llvm::ELF::Elf_Options*>(region.begin());
      const llvm::ELF::Elf_Options* opte =
        reinterpret_cast<const llvm::ELF::Elf_Options*>(region.begin() + size);

      for (const llvm::ELF::Elf_Options* opt = optb; opt < opte; opt += opt->size) {
        switch (opt->kind) {
          default:
            // Nothing to do.
            break;
          case llvm::ELF::ODK_REGINFO:
            if (config().targets().triple().isArch32Bit()) {
              const llvm::ELF::Elf32_RegInfo* reg =
                reinterpret_cast<const llvm::ELF::Elf32_RegInfo*>(opt + 1);
              m_GP0Map[&pInput] = reg->ri_gp_value;
            }
            else {
              const llvm::ELF::Elf64_RegInfo* reg =
                reinterpret_cast<const llvm::ELF::Elf64_RegInfo*>(opt + 1);
              m_GP0Map[&pInput] = reg->ri_gp_value;
            }
            break;
        }
      }
    }

    return true;
  }

  return GNULDBackend::readSection(pInput, pSD);
}

MipsGOT& MipsGNULDBackend::getGOT()
{
  assert(NULL != m_pGOT);
  return *m_pGOT;
}

const MipsGOT& MipsGNULDBackend::getGOT() const
{
  assert(NULL != m_pGOT);
  return *m_pGOT;
}

MipsPLT& MipsGNULDBackend::getPLT()
{
  assert(NULL != m_pPLT);
  return *m_pPLT;
}

const MipsPLT& MipsGNULDBackend::getPLT() const
{
  assert(NULL != m_pPLT);
  return *m_pPLT;
}

MipsGOTPLT& MipsGNULDBackend::getGOTPLT()
{
  assert(NULL != m_pGOTPLT);
  return *m_pGOTPLT;
}

const MipsGOTPLT& MipsGNULDBackend::getGOTPLT() const
{
  assert(NULL != m_pGOTPLT);
  return *m_pGOTPLT;
}

OutputRelocSection& MipsGNULDBackend::getRelPLT()
{
  assert(NULL != m_pRelPlt);
  return *m_pRelPlt;
}

const OutputRelocSection& MipsGNULDBackend::getRelPLT() const
{
  assert(NULL != m_pRelPlt);
  return *m_pRelPlt;
}

OutputRelocSection& MipsGNULDBackend::getRelDyn()
{
  assert(NULL != m_pRelDyn);
  return *m_pRelDyn;
}

const OutputRelocSection& MipsGNULDBackend::getRelDyn() const
{
  assert(NULL != m_pRelDyn);
  return *m_pRelDyn;
}

unsigned int
MipsGNULDBackend::getTargetSectionOrder(const LDSection& pSectHdr) const
{
  const ELFFileFormat* file_format = getOutputFormat();

  if (file_format->hasGOT() && (&pSectHdr == &file_format->getGOT()))
    return SHO_DATA;

  if (file_format->hasGOTPLT() && (&pSectHdr == &file_format->getGOTPLT()))
    return SHO_DATA;

  if (file_format->hasPLT() && (&pSectHdr == &file_format->getPLT()))
    return SHO_PLT;

  return SHO_UNDEFINED;
}

/// finalizeSymbol - finalize the symbol value
bool MipsGNULDBackend::finalizeTargetSymbols()
{
  if (NULL != m_pGpDispSymbol)
    m_pGpDispSymbol->setValue(m_pGOT->getGPDispAddress());

  return true;
}

/// allocateCommonSymbols - allocate common symbols in the corresponding
/// sections. This is called at pre-layout stage.
/// @refer Google gold linker: common.cc: 214
/// FIXME: Mips needs to allocate small common symbol
bool MipsGNULDBackend::allocateCommonSymbols(Module& pModule)
{
  SymbolCategory& symbol_list = pModule.getSymbolTable();

  if (symbol_list.emptyCommons() && symbol_list.emptyFiles() &&
      symbol_list.emptyLocals() && symbol_list.emptyLocalDyns())
    return true;

  SymbolCategory::iterator com_sym, com_end;

  // FIXME: If the order of common symbols is defined, then sort common symbols
  // std::sort(com_sym, com_end, some kind of order);

  // get corresponding BSS LDSection
  ELFFileFormat* file_format = getOutputFormat();
  LDSection& bss_sect = file_format->getBSS();
  LDSection& tbss_sect = file_format->getTBSS();

  // get or create corresponding BSS SectionData
  SectionData* bss_sect_data = NULL;
  if (bss_sect.hasSectionData())
    bss_sect_data = bss_sect.getSectionData();
  else
    bss_sect_data = IRBuilder::CreateSectionData(bss_sect);

  SectionData* tbss_sect_data = NULL;
  if (tbss_sect.hasSectionData())
    tbss_sect_data = tbss_sect.getSectionData();
  else
    tbss_sect_data = IRBuilder::CreateSectionData(tbss_sect);

  // remember original BSS size
  uint64_t bss_offset  = bss_sect.size();
  uint64_t tbss_offset = tbss_sect.size();

  // allocate all local common symbols
  com_end = symbol_list.localEnd();

  for (com_sym = symbol_list.localBegin(); com_sym != com_end; ++com_sym) {
    if (ResolveInfo::Common == (*com_sym)->desc()) {
      // We have to reset the description of the symbol here. When doing
      // incremental linking, the output relocatable object may have common
      // symbols. Therefore, we can not treat common symbols as normal symbols
      // when emitting the regular name pools. We must change the symbols'
      // description here.
      (*com_sym)->resolveInfo()->setDesc(ResolveInfo::Define);
      Fragment* frag = new FillFragment(0x0, 1, (*com_sym)->size());

      if (ResolveInfo::ThreadLocal == (*com_sym)->type()) {
        // allocate TLS common symbol in tbss section
        tbss_offset += ObjectBuilder::AppendFragment(*frag,
                                                     *tbss_sect_data,
                                                     (*com_sym)->value());
        ObjectBuilder::UpdateSectionAlign(tbss_sect, (*com_sym)->value());
        (*com_sym)->setFragmentRef(FragmentRef::Create(*frag, 0));
      }
      // FIXME: how to identify small and large common symbols?
      else {
        bss_offset += ObjectBuilder::AppendFragment(*frag,
                                                    *bss_sect_data,
                                                    (*com_sym)->value());
        ObjectBuilder::UpdateSectionAlign(bss_sect, (*com_sym)->value());
        (*com_sym)->setFragmentRef(FragmentRef::Create(*frag, 0));
      }
    }
  }

  // allocate all global common symbols
  com_end = symbol_list.commonEnd();
  for (com_sym = symbol_list.commonBegin(); com_sym != com_end; ++com_sym) {
    // We have to reset the description of the symbol here. When doing
    // incremental linking, the output relocatable object may have common
    // symbols. Therefore, we can not treat common symbols as normal symbols
    // when emitting the regular name pools. We must change the symbols'
    // description here.
    (*com_sym)->resolveInfo()->setDesc(ResolveInfo::Define);
    Fragment* frag = new FillFragment(0x0, 1, (*com_sym)->size());

    if (ResolveInfo::ThreadLocal == (*com_sym)->type()) {
      // allocate TLS common symbol in tbss section
      tbss_offset += ObjectBuilder::AppendFragment(*frag,
                                                   *tbss_sect_data,
                                                   (*com_sym)->value());
      ObjectBuilder::UpdateSectionAlign(tbss_sect, (*com_sym)->value());
      (*com_sym)->setFragmentRef(FragmentRef::Create(*frag, 0));
    }
    // FIXME: how to identify small and large common symbols?
    else {
      bss_offset += ObjectBuilder::AppendFragment(*frag,
                                                  *bss_sect_data,
                                                  (*com_sym)->value());
      ObjectBuilder::UpdateSectionAlign(bss_sect, (*com_sym)->value());
      (*com_sym)->setFragmentRef(FragmentRef::Create(*frag, 0));
    }
  }

  bss_sect.setSize(bss_offset);
  tbss_sect.setSize(tbss_offset);
  symbol_list.changeCommonsToGlobal();
  return true;
}

uint64_t MipsGNULDBackend::getGP0(const Input& pInput) const
{
  return m_GP0Map.lookup(&pInput);
}

void MipsGNULDBackend::defineGOTSymbol(IRBuilder& pBuilder)
{
  // If we do not reserve any GOT entries, we do not need to re-define GOT
  // symbol.
  if (!m_pGOT->hasGOT1())
    return;

  // define symbol _GLOBAL_OFFSET_TABLE_
  if ( m_pGOTSymbol != NULL ) {
    pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Unresolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     FragmentRef::Create(*(m_pGOT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
  else {
    m_pGOTSymbol = pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     FragmentRef::Create(*(m_pGOT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
}

void MipsGNULDBackend::defineGOTPLTSymbol(IRBuilder& pBuilder)
{
  // define symbol _PROCEDURE_LINKAGE_TABLE_
  if ( m_pPLTSymbol != NULL ) {
    pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Unresolve>(
                     "_PROCEDURE_LINKAGE_TABLE_",
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     FragmentRef::Create(*(m_pPLT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
  else {
    m_pPLTSymbol = pBuilder.AddSymbol<IRBuilder::Force, IRBuilder::Resolve>(
                     "_PROCEDURE_LINKAGE_TABLE_",
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     FragmentRef::Create(*(m_pPLT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
}

/// doCreateProgramHdrs - backend can implement this function to create the
/// target-dependent segments
void MipsGNULDBackend::doCreateProgramHdrs(Module& pModule)
{
  // TODO
}

bool MipsGNULDBackend::relaxRelocation(IRBuilder& pBuilder, Relocation& pRel)
{
  uint64_t sym_value = 0x0;

  LDSymbol* symbol = pRel.symInfo()->outSymbol();
  if (symbol->hasFragRef()) {
    uint64_t value = symbol->fragRef()->getOutputOffset();
    uint64_t addr = symbol->fragRef()->frag()->getParent()->getSection().addr();
    sym_value = addr + value;
  }

  Stub* stub =
    getStubFactory()->create(pRel, sym_value, pBuilder, *getBRIslandFactory());

  if (NULL == stub)
    return false;

  assert(NULL != stub->symInfo());
  // increase the size of .symtab and .strtab
  LDSection& symtab = getOutputFormat()->getSymTab();
  LDSection& strtab = getOutputFormat()->getStrTab();
  symtab.setSize(symtab.size() + sizeof(llvm::ELF::Elf32_Sym));
  strtab.setSize(strtab.size() + stub->symInfo()->nameSize() + 1);

  return true;
}

bool MipsGNULDBackend::doRelax(Module& pModule, IRBuilder& pBuilder,
                               bool& pFinished)
{
  assert(NULL != getStubFactory() && NULL != getBRIslandFactory());

  bool isRelaxed = false;

  for (Module::obj_iterator input = pModule.obj_begin();
       input != pModule.obj_end(); ++input) {
    LDContext* context = (*input)->context();

    for (LDContext::sect_iterator rs = context->relocSectBegin();
         rs != context->relocSectEnd(); ++rs) {
      LDSection* sec = *rs;

      if (LDFileFormat::Ignore == sec->kind() || !sec->hasRelocData())
        continue;

      for (RelocData::iterator reloc = sec->getRelocData()->begin();
           reloc != sec->getRelocData()->end(); ++reloc) {
        if (llvm::ELF::R_MIPS_26 != reloc->type())
          continue;

        if (relaxRelocation(pBuilder, *llvm::cast<Relocation>(reloc)))
          isRelaxed = true;
      }
    }
  }

  SectionData* textData = getOutputFormat()->getText().getSectionData();

  // find the first fragment w/ invalid offset due to stub insertion
  Fragment* invalid = NULL;
  pFinished = true;
  for (BranchIslandFactory::iterator ii = getBRIslandFactory()->begin(),
                                     ie = getBRIslandFactory()->end();
       ii != ie; ++ii)
  {
    BranchIsland& island = *ii;
    if (island.end() == textData->end())
      break;

    Fragment* exit = island.end();
    if ((island.offset() + island.size()) > exit->getOffset()) {
      invalid = exit;
      pFinished = false;
      break;
    }
  }

  // reset the offset of invalid fragments
  while (NULL != invalid) {
    invalid->setOffset(invalid->getPrevNode()->getOffset() +
                       invalid->getPrevNode()->size());
    invalid = invalid->getNextNode();
  }

  // reset the size of .text
  if (isRelaxed)
    getOutputFormat()->getText().setSize(textData->back().getOffset() +
                                         textData->back().size());

  return isRelaxed;
}

bool MipsGNULDBackend::initTargetStubs()
{
  if (NULL == getStubFactory())
    return false;

  getStubFactory()->addPrototype(new MipsLA25Stub(*this));
  return true;
}

bool MipsGNULDBackend::readRelocation(const llvm::ELF::Elf32_Rel& pRel,
                                      Relocation::Type& pType,
                                      uint32_t& pSymIdx,
                                      uint32_t& pOffset) const
{
  return GNULDBackend::readRelocation(pRel, pType, pSymIdx, pOffset);
}

bool MipsGNULDBackend::readRelocation(const llvm::ELF::Elf32_Rela& pRel,
                                      Relocation::Type& pType,
                                      uint32_t& pSymIdx,
                                      uint32_t& pOffset,
                                      int32_t& pAddend) const
{
  return GNULDBackend::readRelocation(pRel, pType, pSymIdx, pOffset, pAddend);
}

bool MipsGNULDBackend::readRelocation(const llvm::ELF::Elf64_Rel& pRel,
                                      Relocation::Type& pType,
                                      uint32_t& pSymIdx,
                                      uint64_t& pOffset) const
{
  uint64_t r_info = 0x0;
  if (llvm::sys::IsLittleEndianHost) {
    pOffset = pRel.r_offset;
    r_info  = pRel.r_info;
  }
  else {
    pOffset = mcld::bswap64(pRel.r_offset);
    r_info  = mcld::bswap64(pRel.r_info);
  }

  // MIPS 64 little endian (we do not support big endian now)
  // has a "special" encoding of r_info relocation
  // field. Instead of one 64 bit little endian number, it is a little
  // endian 32 bit number followed by a 32 bit big endian number.
  pType = mcld::bswap32(r_info >> 32);
  pSymIdx = r_info & 0xffffffff;
  return true;
}

bool MipsGNULDBackend::readRelocation(const llvm::ELF::Elf64_Rela& pRel,
                                      Relocation::Type& pType,
                                      uint32_t& pSymIdx,
                                      uint64_t& pOffset,
                                      int64_t& pAddend) const
{
  uint64_t r_info = 0x0;
  if (llvm::sys::IsLittleEndianHost) {
    pOffset = pRel.r_offset;
    r_info  = pRel.r_info;
    pAddend = pRel.r_addend;
  }
  else {
    pOffset = mcld::bswap64(pRel.r_offset);
    r_info  = mcld::bswap64(pRel.r_info);
    pAddend = mcld::bswap64(pRel.r_addend);
  }

  pType = mcld::bswap32(r_info >> 32);
  pSymIdx = r_info & 0xffffffff;
  return true;
}

void MipsGNULDBackend::emitRelocation(llvm::ELF::Elf32_Rel& pRel,
                                      Relocation::Type pType,
                                      uint32_t pSymIdx,
                                      uint32_t pOffset) const
{
  GNULDBackend::emitRelocation(pRel, pType, pSymIdx, pOffset);
}

void MipsGNULDBackend::emitRelocation(llvm::ELF::Elf32_Rela& pRel,
                                      Relocation::Type pType,
                                      uint32_t pSymIdx,
                                      uint32_t pOffset,
                                      int32_t pAddend) const
{
  GNULDBackend::emitRelocation(pRel, pType, pSymIdx, pOffset, pAddend);
}

void MipsGNULDBackend::emitRelocation(llvm::ELF::Elf64_Rel& pRel,
                                      Relocation::Type pType,
                                      uint32_t pSymIdx,
                                      uint64_t pOffset) const
{
  uint64_t r_info = mcld::bswap32(pType);
  r_info <<= 32;
  r_info |= pSymIdx;

  pRel.r_info = r_info;
  pRel.r_offset = pOffset;
}

void MipsGNULDBackend::emitRelocation(llvm::ELF::Elf64_Rela& pRel,
                                      Relocation::Type pType,
                                      uint32_t pSymIdx,
                                      uint64_t pOffset,
                                      int64_t pAddend) const
{
  uint64_t r_info = mcld::bswap32(pType);
  r_info <<= 32;
  r_info |= pSymIdx;

  pRel.r_info = r_info;
  pRel.r_offset = pOffset;
  pRel.r_addend = pAddend;
}

//===----------------------------------------------------------------------===//
// Mips32GNULDBackend
//===----------------------------------------------------------------------===//
Mips32GNULDBackend::Mips32GNULDBackend(const LinkerConfig& pConfig,
                                       MipsGNUInfo* pInfo)
  : MipsGNULDBackend(pConfig, pInfo)
{}

bool Mips32GNULDBackend::initRelocator()
{
  if (NULL == m_pRelocator)
    m_pRelocator = new Mips32Relocator(*this, config());

  return true;
}

void Mips32GNULDBackend::initTargetSections(Module& pModule,
                                            ObjectBuilder& pBuilder)
{
  MipsGNULDBackend::initTargetSections(pModule, pBuilder);

  if (LinkerConfig::Object == config().codeGenType())
    return;

  ELFFileFormat* fileFormat = getOutputFormat();

  // initialize .got
  LDSection& got = fileFormat->getGOT();
  m_pGOT = new Mips32GOT(got);

  // initialize .got.plt
  LDSection& gotplt = fileFormat->getGOTPLT();
  m_pGOTPLT = new MipsGOTPLT(gotplt);

  // initialize .plt
  LDSection& plt = fileFormat->getPLT();
  m_pPLT = new MipsPLT(plt);
}

size_t Mips32GNULDBackend::getRelEntrySize()
{
  return 8;
}

size_t Mips32GNULDBackend::getRelaEntrySize()
{
  return 12;
}

//===----------------------------------------------------------------------===//
// Mips64GNULDBackend
//===----------------------------------------------------------------------===//
Mips64GNULDBackend::Mips64GNULDBackend(const LinkerConfig& pConfig,
                                       MipsGNUInfo* pInfo)
  : MipsGNULDBackend(pConfig, pInfo)
{}

bool Mips64GNULDBackend::initRelocator()
{
  if (NULL == m_pRelocator)
    m_pRelocator = new Mips64Relocator(*this, config());

  return true;
}

void Mips64GNULDBackend::initTargetSections(Module& pModule,
                                            ObjectBuilder& pBuilder)
{
  MipsGNULDBackend::initTargetSections(pModule, pBuilder);

  if (LinkerConfig::Object == config().codeGenType())
    return;

  ELFFileFormat* fileFormat = getOutputFormat();

  // initialize .got
  LDSection& got = fileFormat->getGOT();
  m_pGOT = new Mips64GOT(got);

  // initialize .got.plt
  LDSection& gotplt = fileFormat->getGOTPLT();
  m_pGOTPLT = new MipsGOTPLT(gotplt);

  // initialize .plt
  LDSection& plt = fileFormat->getPLT();
  m_pPLT = new MipsPLT(plt);
}

size_t Mips64GNULDBackend::getRelEntrySize()
{
  return 16;
}

size_t Mips64GNULDBackend::getRelaEntrySize()
{
  return 24;
}

//===----------------------------------------------------------------------===//
/// createMipsLDBackend - the help funtion to create corresponding MipsLDBackend
///
static TargetLDBackend* createMipsLDBackend(const LinkerConfig& pConfig)
{
  const llvm::Triple& triple = pConfig.targets().triple();

  if (triple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
  }
  if (triple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
  }

  llvm::Triple::ArchType arch = triple.getArch();

  if (llvm::Triple::mips64el == arch)
    return new Mips64GNULDBackend(pConfig, new MipsGNUInfo(triple));

  assert (arch == llvm::Triple::mipsel);
  return new Mips32GNULDBackend(pConfig, new MipsGNUInfo(triple));
}

//===----------------------------------------------------------------------===//
// Force static initialization.
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeMipsLDBackend() {
  mcld::TargetRegistry::RegisterTargetLDBackend(mcld::TheMipselTarget,
                                                createMipsLDBackend);
  mcld::TargetRegistry::RegisterTargetLDBackend(mcld::TheMips64elTarget,
                                                createMipsLDBackend);
}
