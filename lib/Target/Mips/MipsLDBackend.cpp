//===- MipsLDBackend.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Triple.h>
#include <llvm/Support/ELF.h>

#include <mcld/LD/SectionMap.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Target/OutputRelocSection.h>

#include "Mips.h"
#include "MipsELFDynamic.h"
#include "MipsLDBackend.h"
#include "MipsRelocationFactory.h"

enum {
  // The original o32 abi.
  E_MIPS_ABI_O32    = 0x00001000,
  // O32 extended to work on 64 bit architectures. 
  E_MIPS_ABI_O64    = 0x00002000,
  // EABI in 32 bit mode.
  E_MIPS_ABI_EABI32 = 0x00003000,
  // EABI in 64 bit mode.
  E_MIPS_ABI_EABI64 = 0x00004000
};

namespace mcld {

using namespace llvm;

MipsGNULDBackend::MipsGNULDBackend()
  : m_pRelocFactory(NULL),
    m_pGOT(NULL),
    m_pRelDyn(NULL),
    m_pDynamic(NULL)
{
}

MipsGNULDBackend::~MipsGNULDBackend()
{
  if (NULL != m_pRelocFactory)
    delete m_pRelocFactory;
  if (NULL != m_pGOT)
    delete m_pGOT;
  if (NULL != m_pRelDyn)
    delete m_pRelDyn;
  if (NULL != m_pDynamic)
    delete m_pDynamic;
}

bool MipsGNULDBackend::initTargetSectionMap(SectionMap& pSectionMap)
{
  // Nothing to do because we do not support
  // any MIPS specific sections now.
  return true;
}

void MipsGNULDBackend::initTargetSections(MCLinker& pLinker)
{
  // Nothing to do because we do not support
  // any MIPS specific sections now.
}

void MipsGNULDBackend::initTargetSymbols(MCLinker& pLinker)
{
  // Define the symbol _GLOBAL_OFFSET_TABLE_ if there is a symbol with the
  // same name in input
  m_pGOTSymbol = pLinker.defineSymbol<MCLinker::AsRefered, MCLinker::Resolve>(
                   "_GLOBAL_OFFSET_TABLE_",
                   false,
                   ResolveInfo::Object,
                   ResolveInfo::Define,
                   ResolveInfo::Local,
                   0x0,  // size
                   0x0,  // value
                   NULL, // FragRef
                   ResolveInfo::Hidden);
}

bool MipsGNULDBackend::initRelocFactory(const MCLinker& pLinker)
{
  if (NULL == m_pRelocFactory) {
    m_pRelocFactory = new MipsRelocationFactory(1024, *this);
    m_pRelocFactory->setLayout(pLinker.getLayout());
  }
  return true;
}

RelocationFactory* MipsGNULDBackend::getRelocFactory()
{
  assert(NULL != m_pRelocFactory);
  return m_pRelocFactory;
}

void MipsGNULDBackend::scanRelocation(Relocation& pReloc,
                                      const LDSymbol& pInputSym,
                                      MCLinker& pLinker,
                                      const MCLDInfo& pLDInfo,
                                      const Output& pOutput)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  assert(NULL != rsym && "ResolveInfo of relocation not set while scanRelocation");

  // A refernece to symbol _GLOBAL_OFFSET_TABLE_ implies
  // that a .got section is needed.
  if (NULL == m_pGOT && NULL != m_pGOTSymbol) {
    if (rsym == m_pGOTSymbol->resolveInfo()) {
      createGOT(pLinker, pOutput);
    }
  }

  if (rsym->isLocal())
    scanLocalReloc(pReloc, pLinker, pOutput);
  else if (rsym->isGlobal())
    scanGlobalReloc(pReloc, pLinker, pOutput);
}

uint32_t MipsGNULDBackend::machine() const
{
  return llvm::ELF::EM_MIPS;
}

uint8_t MipsGNULDBackend::OSABI() const
{
  return llvm::ELF::ELFOSABI_NONE;
}

uint8_t MipsGNULDBackend::ABIVersion() const
{
  return 0;
}

uint64_t MipsGNULDBackend::flags() const
{
  // TODO: (simon) The correct flag's set depend on command line
  // arguments and flags from input .o files.
  return llvm::ELF::EF_MIPS_ARCH_32R2 |
         llvm::ELF::EF_MIPS_NOREORDER |
         llvm::ELF::EF_MIPS_PIC |
         llvm::ELF::EF_MIPS_CPIC |
         E_MIPS_ABI_O32;
}

bool MipsGNULDBackend::isLittleEndian() const
{
  // Now we support little endian (mipsel) target only.
  return true;
}

unsigned int MipsGNULDBackend::bitclass() const
{
  return 32;
}

void MipsGNULDBackend::doPreLayout(const Output& pOutput,
                                   const MCLDInfo& pInfo,
                                   MCLinker& pLinker)
{
  // when building shared object, the .got section is must.
  if (pOutput.type() == Output::DynObj && NULL == m_pGOT) {
      createGOT(pLinker, pOutput);
  }
}

void MipsGNULDBackend::doPostLayout(const Output& pOutput,
                                    const MCLDInfo& pInfo,
                                    MCLinker& pLinker)
{
  // emit program headers
  if (pOutput.type() == Output::DynObj || pOutput.type() == Output::Exec)
    emitProgramHdrs(pLinker.getLDInfo().output());

  ELFFileFormat* file_format = getOutputFormat(pOutput);

  // apply GOT
  if (file_format->hasGOT()) {
    // Since we already have the size of GOT, m_pGOT should not be NULL.
    assert(NULL != m_pGOT);
#if 0
    // FIXME: (simon)
    if (pOutput.type() == Output::DynObj)
      m_pGOT->applyGOT0(file_format->getDynamic().addr());
    else {
      // executable file and object file should fill with zero.
      m_pGOT->applyGOT0(0);
    }
#endif
  }
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
MipsELFDynamic& MipsGNULDBackend::dynamic()
{
  if (NULL == m_pDynamic)
    m_pDynamic = new MipsELFDynamic(*this);

  return *m_pDynamic;
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
const MipsELFDynamic& MipsGNULDBackend::dynamic() const
{
  assert( NULL != m_pDynamic);
  return *m_pDynamic;
}

uint64_t MipsGNULDBackend::emitSectionData(const Output& pOutput,
                                           const LDSection& pSection,
                                           const MCLDInfo& pInfo,
                                           MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  ELFFileFormat* file_format = getOutputFormat(pOutput);

  if (&pSection == &(file_format->getGOT())) {
    assert(NULL != m_pGOT && "emitSectionData failed, m_pGOT is NULL!");
    uint64_t result = m_pGOT->emit(pRegion);
    pRegion.sync();
    return result;
  }

  llvm::report_fatal_error(llvm::Twine("Unable to emit section `") +
                           pSection.name() +
                           llvm::Twine("'.\n"));
  return 0;
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

unsigned int
MipsGNULDBackend::getTargetSectionOrder(const Output& pOutput,
                                        const LDSection& pSectHdr) const
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  if (&pSectHdr == &file_format->getGOT())
    return SHO_DATA;

  return SHO_UNDEFINED;
}

/// finalizeSymbol - finalize the symbol value
/// If the symbol's reserved field is not zero, MCLinker will call back this
/// function to ask the final value of the symbol
bool MipsGNULDBackend::finalizeSymbol(LDSymbol& pSymbol) const
{
  return false;
}

/// allocateCommonSymbols - allocate common symbols in the corresponding
/// sections.
/// @refer Google gold linker: common.cc: 214
/// FIXME: Mips needs to allocate small common symbol
bool
MipsGNULDBackend::allocateCommonSymbols(const MCLDInfo& pInfo, MCLinker& pLinker) const
{
  // SymbolCategory contains all symbols that must emit to the output files.
  // We are not like Google gold linker, we don't remember symbols before symbol
  // resolution. All symbols in SymbolCategory are already resolved. Therefore, we
  // don't need to care about some symbols may be changed its category due to symbol
  // resolution.
  SymbolCategory& symbol_list = pLinker.getOutputSymbols();

  if (symbol_list.emptyCommons() && symbol_list.emptyLocals())
    return true;

  // addralign := max value of all common symbols
  uint64_t addralign = 0x0;

  // Due to the visibility, some common symbols may be forcefully local.
  SymbolCategory::iterator com_sym, com_end = symbol_list.localEnd();
  for (com_sym = symbol_list.localBegin(); com_sym != com_end; ++com_sym) {
    if (ResolveInfo::Common == (*com_sym)->desc()) {
      if ((*com_sym)->value() > addralign)
        addralign = (*com_sym)->value();
    }
  }

  // global common symbols.
  com_end = symbol_list.commonEnd();
  for (com_sym = symbol_list.commonBegin(); com_sym != com_end; ++com_sym) {
    if ((*com_sym)->value() > addralign)
      addralign = (*com_sym)->value();
  }

  // FIXME: If the order of common symbols is defined, then sort common symbols
  // com_sym = symbol_list.commonBegin();
  // std::sort(com_sym, com_end, some kind of order);

  // get or create corresponding BSS LDSection
  LDSection* bss_sect_hdr = NULL;
  if (ResolveInfo::ThreadLocal == (*com_sym)->type()) {
    bss_sect_hdr = &pLinker.getOrCreateOutputSectHdr(
                                   ".tbss",
                                   LDFileFormat::BSS,
                                   llvm::ELF::SHT_NOBITS,
                                   llvm::ELF::SHF_WRITE | llvm::ELF::SHF_ALLOC);
  }
  else {
    bss_sect_hdr = &pLinker.getOrCreateOutputSectHdr(".bss",
                                   LDFileFormat::BSS,
                                   llvm::ELF::SHT_NOBITS,
                                   llvm::ELF::SHF_WRITE | llvm::ELF::SHF_ALLOC);
  }

  // get or create corresponding BSS MCSectionData
  assert(NULL != bss_sect_hdr);
  llvm::MCSectionData& bss_section = pLinker.getOrCreateSectData(*bss_sect_hdr);

  // allocate all common symbols
  uint64_t offset = bss_sect_hdr->size();

  // allocate all local common symbols
  com_end = symbol_list.localEnd();
  for (com_sym = symbol_list.localBegin(); com_sym != com_end; ++com_sym) {
    if (ResolveInfo::Common == (*com_sym)->desc()) {
      alignAddress(offset, (*com_sym)->value());
      // We have to reset the description of the symbol here. When doing
      // incremental linking, the output relocatable object may have common
      // symbols. Therefore, we can not treat common symbols as normal symbols
      // when emitting the regular name pools. We must change the symbols'
      // description here.
      (*com_sym)->resolveInfo()->setDesc(ResolveInfo::Define);
      llvm::MCFragment* frag = new llvm::MCFillFragment(0x0, 1, (*com_sym)->size(), &bss_section);
      (*com_sym)->setFragmentRef(new MCFragmentRef(*frag, 0));
      offset += (*com_sym)->size();
    }
  }

  // allocate all global common symbols
  com_end = symbol_list.commonEnd();
  for (com_sym = symbol_list.commonBegin(); com_sym != com_end; ++com_sym) {
    alignAddress(offset, (*com_sym)->value());

    // We have to reset the description of the symbol here. When doing
    // incremental linking, the output relocatable object may have common
    // symbols. Therefore, we can not treat common symbols as normal symbols
    // when emitting the regular name pools. We must change the symbols'
    // description here.
    (*com_sym)->resolveInfo()->setDesc(ResolveInfo::Define);
    llvm::MCFragment* frag = new llvm::MCFillFragment(0x0, 1, (*com_sym)->size(), &bss_section);
    (*com_sym)->setFragmentRef(new MCFragmentRef(*frag, 0));
    offset += (*com_sym)->size();
  }

  bss_sect_hdr->setSize(offset);
  symbol_list.changeCommonsToGlobal();
  return true;
}

void MipsGNULDBackend::scanLocalReloc(Relocation& pReloc,
                                      MCLinker& pLinker,
                                      const Output& pOutput)
{
  ResolveInfo* rsym = pReloc.symInfo();

  switch (pReloc.type()){
    case llvm::ELF::R_MIPS_NONE:
    case ELF::R_MIPS_16:
      break;
    case ELF::R_MIPS_32:
      if (Output::DynObj == pOutput.type()) {
        // TODO: (simon) Check section flag SHF_EXECINSTR
        // half_t shndx = rsym->getSectionIndex();
        if (true) {
          if (NULL == m_pRelDyn)
            createRelDyn(pLinker, pOutput);

          m_pRelDyn->reserveEntry(*m_pRelocFactory);
        }
      }
      break;
    case ELF::R_MIPS_REL32:
    case ELF::R_MIPS_26:
    case ELF::R_MIPS_HI16:
    case ELF::R_MIPS_LO16:
    case ELF::R_MIPS_PC16:
    case ELF::R_MIPS_SHIFT5:
    case ELF::R_MIPS_SHIFT6:
    case ELF::R_MIPS_64:
    case ELF::R_MIPS_GOT_PAGE:
    case ELF::R_MIPS_GOT_OFST:
    case ELF::R_MIPS_SUB:
    case ELF::R_MIPS_INSERT_A:
    case ELF::R_MIPS_INSERT_B:
    case ELF::R_MIPS_DELETE:
    case ELF::R_MIPS_HIGHER:
    case ELF::R_MIPS_HIGHEST:
    case ELF::R_MIPS_SCN_DISP:
    case ELF::R_MIPS_REL16:
    case ELF::R_MIPS_ADD_IMMEDIATE:
    case ELF::R_MIPS_PJUMP:
    case ELF::R_MIPS_RELGOT:
    case ELF::R_MIPS_JALR:
    case ELF::R_MIPS_GLOB_DAT:
    case ELF::R_MIPS_COPY:
    case ELF::R_MIPS_JUMP_SLOT:
      break;
    case ELF::R_MIPS_GOT16:
    case ELF::R_MIPS_CALL16:
      if (NULL == m_pGOT)
        createGOT(pLinker, pOutput);

      m_pGOT->reserveEntry();
      break;
    case ELF::R_MIPS_GPREL32:
    case ELF::R_MIPS_GPREL16:
    case ELF::R_MIPS_LITERAL:
      break;
    case ELF::R_MIPS_GOT_DISP:
    case ELF::R_MIPS_GOT_HI16:
    case ELF::R_MIPS_CALL_HI16:
    case ELF::R_MIPS_GOT_LO16:
    case ELF::R_MIPS_CALL_LO16:
      break;
    case ELF::R_MIPS_TLS_DTPMOD32:
    case ELF::R_MIPS_TLS_DTPREL32:
    case ELF::R_MIPS_TLS_DTPMOD64:
    case ELF::R_MIPS_TLS_DTPREL64:
    case ELF::R_MIPS_TLS_GD:
    case ELF::R_MIPS_TLS_LDM:
    case ELF::R_MIPS_TLS_DTPREL_HI16:
    case ELF::R_MIPS_TLS_DTPREL_LO16:
    case ELF::R_MIPS_TLS_GOTTPREL:
    case ELF::R_MIPS_TLS_TPREL32:
    case ELF::R_MIPS_TLS_TPREL64:
    case ELF::R_MIPS_TLS_TPREL_HI16:
    case ELF::R_MIPS_TLS_TPREL_LO16:
      break;
    default:
      llvm::report_fatal_error(llvm::Twine("Unknown relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine("for the local symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
  }
}

void MipsGNULDBackend::scanGlobalReloc(Relocation& pReloc,
                                       MCLinker& pLinker,
                                       const Output& pOutput)
{
  ResolveInfo* rsym = pReloc.symInfo();

  if (isSymbolNeedsPLT(*rsym, pOutput) /* TODO: check the sym hasn't a PLT offset */) {
    if (Output::DynObj == pOutput.type()) {
      // TODO: (simon) Reserve .MIPS.stubs entry
    }
    else if (isSymbolNeedsDynRel(*rsym, pOutput)) {
      // TODO: (simon) Reserve .plt entry
    }
  }

  switch (pReloc.type()){
    case ELF::R_MIPS_NONE:
    case ELF::R_MIPS_INSERT_A:
    case ELF::R_MIPS_INSERT_B:
    case ELF::R_MIPS_DELETE:
    case ELF::R_MIPS_TLS_DTPMOD64:
    case ELF::R_MIPS_TLS_DTPREL64:
    case ELF::R_MIPS_REL16:
    case ELF::R_MIPS_ADD_IMMEDIATE:
    case ELF::R_MIPS_PJUMP:
    case ELF::R_MIPS_RELGOT:
    case ELF::R_MIPS_TLS_TPREL64:
      break;
    case ELF::R_MIPS_32:
    case ELF::R_MIPS_64:
    case ELF::R_MIPS_HI16:
    case ELF::R_MIPS_LO16:
      if (isSymbolNeedsDynRel(*rsym, pOutput)) {
        if (NULL == m_pRelDyn)
          createRelDyn(pLinker, pOutput);

        m_pRelDyn->reserveEntry(*m_pRelocFactory);
      }
      break;
    case ELF::R_MIPS_GOT16:
    case ELF::R_MIPS_CALL16:
    case ELF::R_MIPS_GOT_DISP:
    case ELF::R_MIPS_GOT_HI16:
    case ELF::R_MIPS_CALL_HI16:
    case ELF::R_MIPS_GOT_LO16:
    case ELF::R_MIPS_CALL_LO16:
    case ELF::R_MIPS_GOT_PAGE:
    case ELF::R_MIPS_GOT_OFST:
      if (NULL == m_pGOT)
        createGOT(pLinker, pOutput);

      m_pGOT->reserveEntry();
      break;
    case ELF::R_MIPS_LITERAL:
    case ELF::R_MIPS_GPREL32:
      llvm::report_fatal_error(llvm::Twine("Relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine(" is not defined for the "
                                           "global symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
      break;
    case ELF::R_MIPS_GPREL16:
      break;
    case ELF::R_MIPS_26:
    case ELF::R_MIPS_PC16:
      break;
    case ELF::R_MIPS_16:
    case ELF::R_MIPS_SHIFT5:
    case ELF::R_MIPS_SHIFT6:
    case ELF::R_MIPS_SUB:
    case ELF::R_MIPS_HIGHER:
    case ELF::R_MIPS_HIGHEST:
    case ELF::R_MIPS_SCN_DISP:
      break;
    case ELF::R_MIPS_TLS_DTPREL32:
    case ELF::R_MIPS_TLS_GD:
    case ELF::R_MIPS_TLS_LDM:
    case ELF::R_MIPS_TLS_DTPREL_HI16:
    case ELF::R_MIPS_TLS_DTPREL_LO16:
    case ELF::R_MIPS_TLS_GOTTPREL:
    case ELF::R_MIPS_TLS_TPREL32:
    case ELF::R_MIPS_TLS_TPREL_HI16:
    case ELF::R_MIPS_TLS_TPREL_LO16:
      break;
    case ELF::R_MIPS_REL32:
      break;
    case ELF::R_MIPS_JALR:
      break;
    case ELF::R_MIPS_COPY:
    case ELF::R_MIPS_GLOB_DAT:
    case ELF::R_MIPS_JUMP_SLOT:
      llvm::report_fatal_error(llvm::Twine("Relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine("for the global symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("' should only be seen "
                                           "by the dynamic linker"));
      break;
    default:
      llvm::report_fatal_error(llvm::Twine("Unknown relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine("for the global symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
  }
}

bool MipsGNULDBackend::isSymbolNeedsPLT(ResolveInfo& pSym,
                                        const Output& pOutput) const
{
  // FIXME: (simon) extend conditions
  return Output::DynObj == pOutput.type() &&
         ResolveInfo::Function == pSym.type() &&
         pSym.isDyn() || pSym.isUndef();
}

bool MipsGNULDBackend::isSymbolNeedsDynRel(ResolveInfo& pSym,
                                           const Output& pOutput) const
{
  if(pSym.isUndef() && Output::Exec == pOutput.type())
    return false;
  if(pSym.isAbsolute())
    return false;
  if(Output::DynObj == pOutput.type())
    return true;
  if(pSym.isDyn() || pSym.isUndef())
    return true;

  return false;
}

void MipsGNULDBackend::createGOT(MCLinker& pLinker, const Output& pOutput)
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& got = file_format->getGOT();
  m_pGOT = new MipsGOT(got, pLinker.getOrCreateSectData(got));

  // define symbol _GLOBAL_OFFSET_TABLE_ when .got create
  if( m_pGOTSymbol != NULL ) {
    pLinker.defineSymbol<MCLinker::Force, MCLinker::Unresolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     false,
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     pLinker.getLayout().getFragmentRef(*(m_pGOT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
  else {
    m_pGOTSymbol = pLinker.defineSymbol<MCLinker::Force, MCLinker::Resolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     false,
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     pLinker.getLayout().getFragmentRef(*(m_pGOT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
}

void MipsGNULDBackend::createRelDyn(MCLinker& pLinker, const Output& pOutput)
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  // get .rel.dyn LDSection and create MCSectionData
  LDSection& reldyn = file_format->getRelDyn();
  // create MCSectionData and ARMRelDynSection
  m_pRelDyn = new OutputRelocSection(reldyn,
                                     pLinker.getOrCreateSectData(reldyn),
                                     8);
}

ELFFileFormat* MipsGNULDBackend::getOutputFormat(const Output& pOutput) const
{
  switch (pOutput.type()) {
    case Output::DynObj:
      return getDynObjFileFormat();
    case Output::Exec:
      return getExecFileFormat();
    case Output::Object:
      return NULL;
    default:
      llvm::report_fatal_error(llvm::Twine("Unsupported output file format: ") +
                               llvm::Twine(pOutput.type()));
      return NULL;
  }
}

//===----------------------------------------------------------------------===//
/// createMipsLDBackend - the help funtion to create corresponding MipsLDBackend
///
static TargetLDBackend* createMipsLDBackend(const llvm::Target& pTarget,
                                            const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
  }
  return new MipsGNULDBackend();
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeMipsLDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(mcld::TheMipselTarget,
                                                mcld::createMipsLDBackend);
}
