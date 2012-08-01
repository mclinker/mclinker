//===- MipsLDBackend.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Mips.h"
#include "MipsELFDynamic.h"
#include "MipsLDBackend.h"
#include "MipsRelocationFactory.h"

#include <llvm/ADT/Triple.h>
#include <llvm/Support/ELF.h>

#include <mcld/LD/FillFragment.h>
#include <mcld/LD/SectionMap.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Target/OutputRelocSection.h>

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
}

void MipsGNULDBackend::initTargetSymbols(MCLinker& pLinker, const Output& pOutput)
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
                                      const Output& pOutput,
                                      const LDSection& pSection)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  assert(NULL != rsym && "ResolveInfo of relocation not set while scanRelocation");

  assert(NULL != pSection.getLink());
  if (0 == (pSection.getLink()->flag() & llvm::ELF::SHF_ALLOC)) {
    if (rsym->isLocal()) {
      updateAddend(pReloc, pInputSym, pLinker.getLayout());
    }
    return;
  }

  // A refernece to symbol _GLOBAL_OFFSET_TABLE_ implies
  // that a .got section is needed.
  if (NULL == m_pGOT && NULL != m_pGOTSymbol) {
    if (rsym == m_pGOTSymbol->resolveInfo()) {
      createGOT(pLinker, pOutput);
    }
  }

  // Skip relocation against _gp_disp
  if (strcmp("_gp_disp", pInputSym.name()) == 0)
    return;

  // We test isLocal or if pInputSym is not a dynamic symbol
  // We assume -Bsymbolic to bind all symbols internaly via !rsym->isDyn()
  // Don't put undef symbols into local entries.
  if ((rsym->isLocal() || !isDynamicSymbol(pInputSym, pOutput) ||
      !rsym->isDyn()) && !rsym->isUndef())
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

uint64_t MipsGNULDBackend::defaultTextSegmentAddr() const
{
  return 0x80000;
}

uint64_t MipsGNULDBackend::abiPageSize(const MCLDInfo& pInfo) const
{
  if (pInfo.options().maxPageSize() > 0)
    return pInfo.options().maxPageSize();
  else
    return static_cast<uint64_t>(0x10000);
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
                                           const Layout& pLayout,
                                           MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  const ELFFileFormat* file_format = getOutputFormat(pOutput);

  if (&pSection == &(file_format->getGOT())) {
    assert(NULL != m_pGOT && "emitSectionData failed, m_pGOT is NULL!");
    uint64_t result = m_pGOT->emit(pRegion);
    return result;
  }

  fatal(diag::unrecognized_output_sectoin)
          << pSection.name()
          << "mclinker@googlegroups.com";
  return 0;
}
/// isGlobalGOTSymbol - return true if the symbol is the global GOT entry.
bool MipsGNULDBackend::isGlobalGOTSymbol(const LDSymbol& pSymbol) const
{
  return std::find(m_GlobalGOTSyms.begin(),
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

    if (isGlobalGOTSymbol(**symbol))
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

    // Make sure this golbal GOT entry is a dynamic symbol.
    // If not, something is wrong earlier when putting this symbol into
    //  global GOT.
    if (!isDynamicSymbol(**symbol, pOutput))
      fatal(diag::mips_got_symbol) << (*symbol)->name();

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
  if (Output::DynObj == pOutput.type())
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
                                        const LDSection& pSectHdr,
                                        const MCLDInfo& pInfo) const
{
  const ELFFileFormat* file_format = getOutputFormat(pOutput);

  if (&pSectHdr == &file_format->getGOT())
    return SHO_DATA;

  return SHO_UNDEFINED;
}

/// finalizeSymbol - finalize the symbol value
bool MipsGNULDBackend::finalizeTargetSymbols(MCLinker& pLinker, const Output& pOutput)
{
  if (NULL != m_pGpDispSymbol)
    m_pGpDispSymbol->setValue(m_pGOT->getSection().addr() + 0x7FF0);
  return true;
}

/// allocateCommonSymbols - allocate common symbols in the corresponding
/// sections.
/// @refer Google gold linker: common.cc: 214
/// FIXME: Mips needs to allocate small common symbol
bool
MipsGNULDBackend::allocateCommonSymbols(const MCLDInfo& pInfo, MCLinker& pLinker) const
{
  SymbolCategory& symbol_list = pLinker.getOutputSymbols();

  if (symbol_list.emptyCommons() && symbol_list.emptyLocals())
    return true;

  SymbolCategory::iterator com_sym, com_end;

  // FIXME: If the order of common symbols is defined, then sort common symbols
  // std::sort(com_sym, com_end, some kind of order);

  // get or create corresponding BSS LDSection
  LDSection* bss_sect = &pLinker.getOrCreateOutputSectHdr(".bss",
                                   LDFileFormat::BSS,
                                   llvm::ELF::SHT_NOBITS,
                                   llvm::ELF::SHF_WRITE | llvm::ELF::SHF_ALLOC);

  LDSection* tbss_sect = &pLinker.getOrCreateOutputSectHdr(
                                   ".tbss",
                                   LDFileFormat::BSS,
                                   llvm::ELF::SHT_NOBITS,
                                   llvm::ELF::SHF_WRITE | llvm::ELF::SHF_ALLOC);

  // FIXME: .sbss amd .lbss currently unused.
  /*
  LDSection* sbss_sect = &pLinker.getOrCreateOutputSectHdr(
                                   ".sbss",
                                   LDFileFormat::BSS,
                                   llvm::ELF::SHT_NOBITS,
                                   llvm::ELF::SHF_WRITE | llvm::ELF::SHF_ALLOC |
                                   llvm::ELF::SHF_MIPS_GPREL);

  LDSection* lbss_sect = &pLinker.getOrCreateOutputSectHdr(
                                   ".lbss",
                                   LDFileFormat::BSS,
                                   llvm::ELF::SHT_NOBITS,
                                   llvm::ELF::SHF_WRITE | llvm::ELF::SHF_ALLOC |
                                   llvm::ELF::SHF_MIPS_LOCAL);
  */

  assert(NULL != bss_sect && NULL != tbss_sect);

  // get or create corresponding BSS SectionData
  SectionData& bss_sect_data = pLinker.getOrCreateSectData(*bss_sect);
  SectionData& tbss_sect_data = pLinker.getOrCreateSectData(*tbss_sect);

  // remember original BSS size
  uint64_t bss_offset  = bss_sect->size();
  uint64_t tbss_offset = tbss_sect->size();

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
      (*com_sym)->setFragmentRef(new FragmentRef(*frag, 0));

      if (ResolveInfo::ThreadLocal == (*com_sym)->type()) {
        // allocate TLS common symbol in tbss section
        tbss_offset += pLinker.getLayout().appendFragment(*frag,
                                                          tbss_sect_data,
                                                          (*com_sym)->value());
      }
      // FIXME: how to identify small and large common symbols?
      else {
        bss_offset += pLinker.getLayout().appendFragment(*frag,
                                                         bss_sect_data,
                                                         (*com_sym)->value());
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
    (*com_sym)->setFragmentRef(new FragmentRef(*frag, 0));

    if (ResolveInfo::ThreadLocal == (*com_sym)->type()) {
      // allocate TLS common symbol in tbss section
      tbss_offset += pLinker.getLayout().appendFragment(*frag,
                                                        tbss_sect_data,
                                                        (*com_sym)->value());
    }
    // FIXME: how to identify small and large common symbols?
    else {
      bss_offset += pLinker.getLayout().appendFragment(*frag,
                                                       bss_sect_data,
                                                       (*com_sym)->value());
    }
  }

  bss_sect->setSize(bss_offset);
  tbss_sect->setSize(tbss_offset);
  symbol_list.changeCommonsToGlobal();
  return true;
}

void MipsGNULDBackend::updateAddend(Relocation& pReloc,
                                   const LDSymbol& pInputSym,
                                   const Layout& pLayout) const
{
  // Update value keep in addend if we meet a section symbol
  if (pReloc.symInfo()->type() == ResolveInfo::Section) {
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

        // Remeber this rsym is a local GOT entry (as if it needs an entry).
        // Actually we don't allocate an GOT entry.
        if (NULL == m_pGOT)
          createGOT(pLinker, pOutput);
        m_pGOT->setLocal(rsym);
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

      // For got16 section based relocations, we need to reserve got entries.
      if (rsym->type() == ResolveInfo::Section) {
        m_pGOT->reserveLocalEntry();
        // Remeber this rsym is a local GOT entry
        m_pGOT->setLocal(rsym);
        return;
      }

      if (!(rsym->reserved() & MipsGNULDBackend::ReserveGot)) {
        m_pGOT->reserveLocalEntry();
        rsym->setReserved(rsym->reserved() | ReserveGot);
        // Remeber this rsym is a local GOT entry
        m_pGOT->setLocal(rsym);
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
      fatal(diag::unknown_relocation) << (int)pReloc.type()
                                      << pReloc.symInfo()->name();
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
      if (symbolNeedsDynRel(*rsym, false, pLDInfo, pOutput, true)) {
        if (NULL == m_pRelDyn)
          createRelDyn(pLinker, pOutput);

        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        rsym->setReserved(rsym->reserved() | ReserveRel);

        // Remeber this rsym is a global GOT entry (as if it needs an entry).
        // Actually we don't allocate an GOT entry.
        if (NULL == m_pGOT)
          createGOT(pLinker, pOutput);
        m_pGOT->setGlobal(rsym);
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
        // Remeber this rsym is a global GOT entry
        m_pGOT->setGlobal(rsym);
      }
      break;
    case llvm::ELF::R_MIPS_LITERAL:
    case llvm::ELF::R_MIPS_GPREL32:
      fatal(diag::invalid_global_relocation) << (int)pReloc.type()
                                             << pReloc.symInfo()->name();
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
      fatal(diag::dynamic_relocation) << (int)pReloc.type();
      break;
    default:
      fatal(diag::unknown_relocation) << (int)pReloc.type()
                                      << pReloc.symInfo()->name();
  }
}

void MipsGNULDBackend::createGOT(MCLinker& pLinker, const Output& pOutput)
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& got = file_format->getGOT();
  m_pGOT = new MipsGOT(got, pLinker.getOrCreateSectData(got));

  // define symbol _GLOBAL_OFFSET_TABLE_ when .got create
  if ( m_pGOTSymbol != NULL ) {
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

  // get .rel.dyn LDSection and create SectionData
  LDSection& reldyn = file_format->getRelDyn();
  // create SectionData and ARMRelDynSection
  m_pRelDyn = new OutputRelocSection(reldyn,
                                     pLinker.getOrCreateSectData(reldyn),
                                     8);
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
