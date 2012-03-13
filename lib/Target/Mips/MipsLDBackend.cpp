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

MipsGNULDBackend::MipsGNULDBackend()
  : m_pRelocFactory(NULL),
    m_pGOT(NULL),
    m_pRelDyn(NULL),
    m_pDynamic(NULL),
    m_pGOTSymbol(NULL),
    m_pGpDispSymbol(NULL)
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

  m_pGpDispSymbol = pLinker.defineSymbol<MCLinker::AsRefered, MCLinker::Resolve>(
                   "_gp_disp",
                   false,
                   ResolveInfo::Section,
                   ResolveInfo::Define,
                   ResolveInfo::Absolute,
                   0x0,  // size
                   0x0,  // value
                   NULL, // FragRef
                   ResolveInfo::Default);

  if (NULL != m_pGpDispSymbol) {
    m_pGpDispSymbol->resolveInfo()->setReserved(ReserveGpDisp);
  }
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
    scanLocalReloc(pReloc, pInputSym, pLinker, pLDInfo, pOutput);
  else
    scanGlobalReloc(pReloc, pInputSym, pLinker, pLDInfo, pOutput);
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
    return result;
  }

  llvm::report_fatal_error(llvm::Twine("Unable to emit section `") +
                           pSection.name() +
                           llvm::Twine("'.\n"));
  return 0;
}
/// isGOTSymbol - return true if the symbol is the GOT entry.
bool MipsGNULDBackend::isGOTSymbol(const LDSymbol& pSymbol) const
{
  return std::find(m_LocalGOTSyms.begin(),
                   m_LocalGOTSyms.end(), &pSymbol) != m_LocalGOTSyms.end() ||
         std::find(m_GlobalGOTSyms.begin(),
                   m_GlobalGOTSyms.end(), &pSymbol) != m_GlobalGOTSyms.end();
}

/// emitDynamicSymbol - emit dynamic symbol.
void MipsGNULDBackend::emitDynamicSymbol(llvm::ELF::Elf32_Sym& sym32,
                                         Output& pOutput,
                                         LDSymbol& pSymbol,
                                         const Layout& pLayout,
                                         char* strtab,
                                         size_t strtabsize,
                                         size_t symtabIdx)
{
  // maintain output's symbol and index map
  bool sym_exist = false;
  HashTableType::entry_type* entry = 0;
  entry = m_pSymIndexMap->insert(&pSymbol, sym_exist);
  entry->setValue(symtabIdx);

  // FIXME: check the endian between host and target
  // write out symbol
  sym32.st_name  = strtabsize;
  sym32.st_value = pSymbol.value();
  sym32.st_size  = getSymbolSize(pSymbol);
  sym32.st_info  = getSymbolInfo(pSymbol);
  sym32.st_other = pSymbol.visibility();
  sym32.st_shndx = getSymbolShndx(pSymbol, pLayout);
  // write out string
  strcpy((strtab + strtabsize), pSymbol.name());
}

/// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
///
/// the size of these tables should be computed before layout
/// layout should computes the start offset of these tables
void MipsGNULDBackend::emitDynNamePools(Output& pOutput,
                                        SymbolCategory& pSymbols,
                                        const Layout& pLayout,
                                        const MCLDInfo& pLDInfo)
{
  assert(pOutput.hasMemArea());
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& symtab_sect = file_format->getDynSymTab();
  LDSection& strtab_sect = file_format->getDynStrTab();
  LDSection& hash_sect   = file_format->getHashTab();
  LDSection& dyn_sect    = file_format->getDynamic();

  MemoryRegion* symtab_region = pOutput.memArea()->request(symtab_sect.offset(),
                                                           symtab_sect.size());
  MemoryRegion* strtab_region = pOutput.memArea()->request(strtab_sect.offset(),
                                                           strtab_sect.size());
  MemoryRegion* hash_region = pOutput.memArea()->request(hash_sect.offset(),
                                                         hash_sect.size());
  MemoryRegion* dyn_region = pOutput.memArea()->request(dyn_sect.offset(),
                                                        dyn_sect.size());
  // set up symtab_region
  llvm::ELF::Elf32_Sym* symtab32 = NULL;
  symtab32 = (llvm::ELF::Elf32_Sym*)symtab_region->start();

  symtab32[0].st_name  = 0;
  symtab32[0].st_value = 0;
  symtab32[0].st_size  = 0;
  symtab32[0].st_info  = 0;
  symtab32[0].st_other = 0;
  symtab32[0].st_shndx = 0;

  // set up strtab_region
  char* strtab = (char*)strtab_region->start();
  strtab[0] = '\0';

  bool sym_exist = false;
  HashTableType::entry_type* entry = 0;

  // add index 0 symbol into SymIndexMap
  entry = m_pSymIndexMap->insert(NULL, sym_exist);
  entry->setValue(0);

  size_t symtabIdx = 1;
  size_t strtabsize = 1;

  // emit of .dynsym, and .dynstr except GOT entries
  for (SymbolCategory::iterator symbol = pSymbols.begin(),
       sym_end = pSymbols.end(); symbol != sym_end; ++symbol) {
    if (!isDynamicSymbol(**symbol, pOutput))
      continue;

    if (isGOTSymbol(**symbol))
      continue;

    emitDynamicSymbol(symtab32[symtabIdx], pOutput, **symbol, pLayout, strtab,
                      strtabsize, symtabIdx);

    // sum up counters
    ++symtabIdx;
    strtabsize += (*symbol)->nameSize() + 1;
  }

  // emit global GOT
  for (std::vector<LDSymbol*>::const_iterator symbol = m_GlobalGOTSyms.begin(),
       symbol_end = m_GlobalGOTSyms.end();
       symbol != symbol_end; ++symbol) {

    emitDynamicSymbol(symtab32[symtabIdx], pOutput, **symbol, pLayout, strtab,
                      strtabsize, symtabIdx);

    // sum up counters
    ++symtabIdx;
    strtabsize += (*symbol)->nameSize() + 1;
  }

  // emit DT_NEED
  // add DT_NEED strings into .dynstr
  // Rules:
  //   1. ignore --no-add-needed
  //   2. force count in --no-as-needed
  //   3. judge --as-needed
  ELFDynamic::iterator dt_need = dynamic().needBegin();
  InputTree::const_bfs_iterator input, inputEnd = pLDInfo.inputs().bfs_end();
  for (input = pLDInfo.inputs().bfs_begin(); input != inputEnd; ++input) {
    if (Input::DynObj == (*input)->type()) {
      // --add-needed
      if ((*input)->attribute()->isAddNeeded()) {
        // --no-as-needed
        if (!(*input)->attribute()->isAsNeeded()) {
          strcpy((strtab + strtabsize), (*input)->name().c_str());
          (*dt_need)->setValue(llvm::ELF::DT_NEEDED, strtabsize);
          strtabsize += (*input)->name().size() + 1;
          ++dt_need;
        }
        // --as-needed
        else if ((*input)->isNeeded()) {
          strcpy((strtab + strtabsize), (*input)->name().c_str());
          (*dt_need)->setValue(llvm::ELF::DT_NEEDED, strtabsize);
          strtabsize += (*input)->name().size() + 1;
          ++dt_need;
        }
      }
    }
  } // for

  // emit soname
  // initialize value of ELF .dynamic section
  dynamic().applySoname(strtabsize);
  dynamic().applyEntries(pLDInfo, *file_format);
  dynamic().emit(dyn_sect, *dyn_region);

  strcpy((strtab + strtabsize), pOutput.name().c_str());
  strtabsize += pOutput.name().size() + 1;

  // emit hash table
  // FIXME: this verion only emit SVR4 hash section.
  //        Please add GNU new hash section

  // both 32 and 64 bits hash table use 32-bit entry
  // set up hash_region
  uint32_t* word_array = (uint32_t*)hash_region->start();
  uint32_t& nbucket = word_array[0];
  uint32_t& nchain  = word_array[1];

  nbucket = getHashBucketCount(symtabIdx, false);
  nchain  = symtabIdx;

  uint32_t* bucket = (word_array + 2);
  uint32_t* chain  = (bucket + nbucket);

  // initialize bucket
  bzero((void*)bucket, nbucket);

  StringHash<ELF> hash_func;

  for (size_t sym_idx=0; sym_idx < symtabIdx; ++sym_idx) {
    llvm::StringRef name(strtab + symtab32[sym_idx].st_name);
    size_t bucket_pos = hash_func(name) % nbucket;
    chain[sym_idx] = bucket[bucket_pos];
    bucket[bucket_pos] = sym_idx;
  }

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
  if (&pSymbol == m_pGpDispSymbol) {
    m_pGpDispSymbol->setValue(m_pGOT->getSection().addr() + 0x7FF0);
    return true;
  }
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

void MipsGNULDBackend::updateAddend(Relocation& pReloc,
                                   const LDSymbol& pInputSym,
                                   const Layout& pLayout) const
{
  // Update value keep in addend if we meet a section symbol
  if(pReloc.symInfo()->type() == ResolveInfo::Section) {
    pReloc.setAddend(pLayout.getOutputOffset(
                     *pInputSym.fragRef()) + pReloc.addend());
  }
}

void MipsGNULDBackend::scanLocalReloc(Relocation& pReloc,
                                      const LDSymbol& pInputSym,
                                      MCLinker& pLinker,
                                      const MCLDInfo& pLDInfo,
                                      const Output& pOutput)
{
  ResolveInfo* rsym = pReloc.symInfo();

  updateAddend(pReloc, pInputSym, pLinker.getLayout());

  switch (pReloc.type()){
    case llvm::ELF::R_MIPS_NONE:
    case llvm::ELF::R_MIPS_16:
      break;
    case llvm::ELF::R_MIPS_32:
      if (Output::DynObj == pOutput.type()) {
        // TODO: (simon) The gold linker does not create an entry in .rel.dyn
        // section if the symbol section flags contains SHF_EXECINSTR.
        // 1. Find the reason of this condition.
        // 2. Check this condition here.
        if (NULL == m_pRelDyn)
          createRelDyn(pLinker, pOutput);

        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      break;
    case llvm::ELF::R_MIPS_REL32:
    case llvm::ELF::R_MIPS_26:
    case llvm::ELF::R_MIPS_HI16:
    case llvm::ELF::R_MIPS_LO16:
    case llvm::ELF::R_MIPS_PC16:
    case llvm::ELF::R_MIPS_SHIFT5:
    case llvm::ELF::R_MIPS_SHIFT6:
    case llvm::ELF::R_MIPS_64:
    case llvm::ELF::R_MIPS_GOT_PAGE:
    case llvm::ELF::R_MIPS_GOT_OFST:
    case llvm::ELF::R_MIPS_SUB:
    case llvm::ELF::R_MIPS_INSERT_A:
    case llvm::ELF::R_MIPS_INSERT_B:
    case llvm::ELF::R_MIPS_DELETE:
    case llvm::ELF::R_MIPS_HIGHER:
    case llvm::ELF::R_MIPS_HIGHEST:
    case llvm::ELF::R_MIPS_SCN_DISP:
    case llvm::ELF::R_MIPS_REL16:
    case llvm::ELF::R_MIPS_ADD_IMMEDIATE:
    case llvm::ELF::R_MIPS_PJUMP:
    case llvm::ELF::R_MIPS_RELGOT:
    case llvm::ELF::R_MIPS_JALR:
    case llvm::ELF::R_MIPS_GLOB_DAT:
    case llvm::ELF::R_MIPS_COPY:
    case llvm::ELF::R_MIPS_JUMP_SLOT:
      break;
    case llvm::ELF::R_MIPS_GOT16:
    case llvm::ELF::R_MIPS_CALL16:
      if (NULL == m_pGOT)
        createGOT(pLinker, pOutput);

      if (!(rsym->reserved() & MipsGNULDBackend::ReserveGot)) {
        m_pGOT->reserveLocalEntry();
        rsym->setReserved(rsym->reserved() | ReserveGot);
        m_LocalGOTSyms.push_back(rsym->outSymbol());
      }
      break;
    case llvm::ELF::R_MIPS_GPREL32:
    case llvm::ELF::R_MIPS_GPREL16:
    case llvm::ELF::R_MIPS_LITERAL:
      break;
    case llvm::ELF::R_MIPS_GOT_DISP:
    case llvm::ELF::R_MIPS_GOT_HI16:
    case llvm::ELF::R_MIPS_CALL_HI16:
    case llvm::ELF::R_MIPS_GOT_LO16:
    case llvm::ELF::R_MIPS_CALL_LO16:
      break;
    case llvm::ELF::R_MIPS_TLS_DTPMOD32:
    case llvm::ELF::R_MIPS_TLS_DTPREL32:
    case llvm::ELF::R_MIPS_TLS_DTPMOD64:
    case llvm::ELF::R_MIPS_TLS_DTPREL64:
    case llvm::ELF::R_MIPS_TLS_GD:
    case llvm::ELF::R_MIPS_TLS_LDM:
    case llvm::ELF::R_MIPS_TLS_DTPREL_HI16:
    case llvm::ELF::R_MIPS_TLS_DTPREL_LO16:
    case llvm::ELF::R_MIPS_TLS_GOTTPREL:
    case llvm::ELF::R_MIPS_TLS_TPREL32:
    case llvm::ELF::R_MIPS_TLS_TPREL64:
    case llvm::ELF::R_MIPS_TLS_TPREL_HI16:
    case llvm::ELF::R_MIPS_TLS_TPREL_LO16:
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
                                       const LDSymbol& pInputSym,
                                       MCLinker& pLinker,
                                       const MCLDInfo& pLDInfo,
                                       const Output& pOutput)
{
  ResolveInfo* rsym = pReloc.symInfo();

  switch (pReloc.type()){
    case llvm::ELF::R_MIPS_NONE:
    case llvm::ELF::R_MIPS_INSERT_A:
    case llvm::ELF::R_MIPS_INSERT_B:
    case llvm::ELF::R_MIPS_DELETE:
    case llvm::ELF::R_MIPS_TLS_DTPMOD64:
    case llvm::ELF::R_MIPS_TLS_DTPREL64:
    case llvm::ELF::R_MIPS_REL16:
    case llvm::ELF::R_MIPS_ADD_IMMEDIATE:
    case llvm::ELF::R_MIPS_PJUMP:
    case llvm::ELF::R_MIPS_RELGOT:
    case llvm::ELF::R_MIPS_TLS_TPREL64:
      break;
    case llvm::ELF::R_MIPS_32:
    case llvm::ELF::R_MIPS_64:
    case llvm::ELF::R_MIPS_HI16:
    case llvm::ELF::R_MIPS_LO16:
      if (isSymbolNeedsDynRel(*rsym, pOutput)) {
        if (NULL == m_pRelDyn)
          createRelDyn(pLinker, pOutput);

        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      break;
    case llvm::ELF::R_MIPS_GOT16:
    case llvm::ELF::R_MIPS_CALL16:
    case llvm::ELF::R_MIPS_GOT_DISP:
    case llvm::ELF::R_MIPS_GOT_HI16:
    case llvm::ELF::R_MIPS_CALL_HI16:
    case llvm::ELF::R_MIPS_GOT_LO16:
    case llvm::ELF::R_MIPS_CALL_LO16:
    case llvm::ELF::R_MIPS_GOT_PAGE:
    case llvm::ELF::R_MIPS_GOT_OFST:
      if (NULL == m_pGOT)
        createGOT(pLinker, pOutput);

      if (!(rsym->reserved() & MipsGNULDBackend::ReserveGot)) {
        m_pGOT->reserveGlobalEntry();
        rsym->setReserved(rsym->reserved() | ReserveGot);
        m_GlobalGOTSyms.push_back(rsym->outSymbol());
      }
      break;
    case llvm::ELF::R_MIPS_LITERAL:
    case llvm::ELF::R_MIPS_GPREL32:
      llvm::report_fatal_error(llvm::Twine("Relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine(" is not defined for the "
                                           "global symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
      break;
    case llvm::ELF::R_MIPS_GPREL16:
      break;
    case llvm::ELF::R_MIPS_26:
    case llvm::ELF::R_MIPS_PC16:
      break;
    case llvm::ELF::R_MIPS_16:
    case llvm::ELF::R_MIPS_SHIFT5:
    case llvm::ELF::R_MIPS_SHIFT6:
    case llvm::ELF::R_MIPS_SUB:
    case llvm::ELF::R_MIPS_HIGHER:
    case llvm::ELF::R_MIPS_HIGHEST:
    case llvm::ELF::R_MIPS_SCN_DISP:
      break;
    case llvm::ELF::R_MIPS_TLS_DTPREL32:
    case llvm::ELF::R_MIPS_TLS_GD:
    case llvm::ELF::R_MIPS_TLS_LDM:
    case llvm::ELF::R_MIPS_TLS_DTPREL_HI16:
    case llvm::ELF::R_MIPS_TLS_DTPREL_LO16:
    case llvm::ELF::R_MIPS_TLS_GOTTPREL:
    case llvm::ELF::R_MIPS_TLS_TPREL32:
    case llvm::ELF::R_MIPS_TLS_TPREL_HI16:
    case llvm::ELF::R_MIPS_TLS_TPREL_LO16:
      break;
    case llvm::ELF::R_MIPS_REL32:
      break;
    case llvm::ELF::R_MIPS_JALR:
      break;
    case llvm::ELF::R_MIPS_COPY:
    case llvm::ELF::R_MIPS_GLOB_DAT:
    case llvm::ELF::R_MIPS_JUMP_SLOT:
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
  return (Output::DynObj == pOutput.type() &&
         ResolveInfo::Function == pSym.type() &&
         (pSym.isDyn() || pSym.isUndef()));
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
  llvm::Triple theTriple(pTriple);
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
