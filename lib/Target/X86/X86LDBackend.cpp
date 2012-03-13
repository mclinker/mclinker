//===- X86LDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "X86.h"
#include "X86ELFDynamic.h"
#include "X86LDBackend.h"
#include "X86RelocationFactory.h"

#include <llvm/ADT/Triple.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/SectionMap.h>
#include <mcld/MC/MCRegionFragment.h>

#include <cstring>

using namespace mcld;

X86GNULDBackend::X86GNULDBackend()
  : m_pRelocFactory(NULL),
    m_pGOT(NULL),
    m_pPLT(NULL),
    m_pRelDyn(NULL),
    m_pRelPLT(NULL),
    m_pDynamic(NULL) {
}

X86GNULDBackend::~X86GNULDBackend()
{
  if (NULL != m_pRelocFactory)
    delete m_pRelocFactory;
  if (NULL != m_pGOT)
    delete m_pGOT;
  if (NULL != m_pPLT)
    delete m_pPLT;
  if (NULL !=m_pRelDyn)
    delete m_pRelDyn;
  if (NULL != m_pRelPLT)
    delete m_pRelPLT;
  if (NULL != m_pDynamic)
    delete m_pDynamic;
}

RelocationFactory* X86GNULDBackend::getRelocFactory()
{
  assert(NULL != m_pRelocFactory);
  return m_pRelocFactory;
}

bool X86GNULDBackend::initRelocFactory(const MCLinker& pLinker)
{
  if (NULL == m_pRelocFactory) {
    m_pRelocFactory = new X86RelocationFactory(1024, *this);
    m_pRelocFactory->setLayout(pLinker.getLayout());
  }
  return true;
}

void X86GNULDBackend::doPreLayout(const Output& pOutput,
                                  const MCLDInfo& pInfo,
                                  MCLinker& pLinker)
{
  // when building shared object, the .got section is needed
  if(pOutput.type() == Output::DynObj && (NULL == m_pGOT))
      createX86GOT(pLinker, pOutput);
}

void X86GNULDBackend::doPostLayout(const Output& pOutput,
                                   const MCLDInfo& pInfo,
                                   MCLinker& pLinker)
{
  // emit program headers
  if(pOutput.type() == Output::DynObj || pOutput.type() == Output::Exec)
    emitProgramHdrs(pLinker.getLDInfo().output());
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
X86ELFDynamic& X86GNULDBackend::dynamic()
{
  if (NULL == m_pDynamic)
    m_pDynamic = new X86ELFDynamic(*this);

  return *m_pDynamic;
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
const X86ELFDynamic& X86GNULDBackend::dynamic() const
{
  assert( NULL != m_pDynamic);
  return *m_pDynamic;
}

void X86GNULDBackend::createX86GOT(MCLinker& pLinker, const Output& pOutput)
{
  // get .got LDSection and create MCSectionData
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& got = file_format->getGOT();
  m_pGOT = new X86GOT(got, pLinker.getOrCreateSectData(got));

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

void X86GNULDBackend::createX86PLTandRelPLT(MCLinker& pLinker,
                                            const Output& pOutput)
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& plt = file_format->getPLT();
  LDSection& relplt = file_format->getRelPlt();
  // create MCSectionData and X86PLT
  m_pPLT = new X86PLT(plt, pLinker.getOrCreateSectData(plt), *m_pGOT, pOutput);

  // set info of .rel.plt to .plt
  relplt.setLink(&plt);
  // create MCSectionData and X86RelDynSection
  m_pRelPLT = new OutputRelocSection(relplt,
                                     pLinker.getOrCreateSectData(relplt),
                                     8);
}

void X86GNULDBackend::createX86RelDyn(MCLinker& pLinker,
                                      const Output& pOutput)
{
  // get .rel.dyn LDSection and create MCSectionData
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& reldyn = file_format->getRelDyn();
  // create MCSectionData and X86RelDynSection
  m_pRelDyn = new OutputRelocSection(reldyn,
                                     pLinker.getOrCreateSectData(reldyn),
                                     8);
}

ELFFileFormat* X86GNULDBackend::getOutputFormat(const Output& pOutput) const
{
  switch (pOutput.type()) {
    case Output::DynObj:
      return getDynObjFileFormat();
    case Output::Exec:
      return getExecFileFormat();
    // FIXME: We do not support building .o now
    case Output::Object:
    default:
      llvm::report_fatal_error(llvm::Twine("Unsupported output file format: ") +
                               llvm::Twine(pOutput.type()));
      return NULL;
  }
}

bool X86GNULDBackend::isSymbolNeedsPLT(const ResolveInfo& pSym,
                                       const MCLDInfo& pLDInfo,
                                       const Output& pOutput) const
{
  return((Output::DynObj == pOutput.type())
         &&(ResolveInfo::Function == pSym.type())
         &&(pSym.isDyn() || pSym.isUndef() ||
            isSymbolPreemptible(pSym, pLDInfo, pOutput))
        );
}

bool X86GNULDBackend::isSymbolNeedsDynRel(const ResolveInfo& pSym,
                                          const Output& pOutput,
                                          bool isAbsReloc) const
{
  if(pSym.isUndef() && (pOutput.type()==Output::Exec))
    return false;
  if(pSym.isAbsolute())
    return false;
  if(pOutput.type()==Output::DynObj && isAbsReloc)
    return true;
  if(pSym.isDyn() || pSym.isUndef())
    return true;

  return false;
}

bool X86GNULDBackend::isSymbolPreemptible(const ResolveInfo& pSym,
                                         const MCLDInfo& pLDInfo,
                                         const Output& pOutput) const
{
  if(pSym.other() != ResolveInfo::Default)
    return false;

  if(pOutput.type() != Output::DynObj)
    return false;

  if(pLDInfo.options().Bsymbolic())
    return false;

  return true;
}

void X86GNULDBackend::updateAddend(Relocation& pReloc,
                                   const LDSymbol& pInputSym,
                                   const Layout& pLayout) const
{
  // Update value keep in addend if we meet a section symbol
  if(pReloc.symInfo()->type() == ResolveInfo::Section) {
    pReloc.setAddend(pLayout.getOutputOffset(
                     *pInputSym.fragRef()) + pReloc.addend());
  }
}

void X86GNULDBackend::scanLocalReloc(Relocation& pReloc,
                                     const LDSymbol& pInputSym,
                                     MCLinker& pLinker,
                                     const MCLDInfo& pLDInfo,
                                     const Output& pOutput)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

  updateAddend(pReloc, pInputSym, pLinker.getLayout());

  switch(pReloc.type()){

    case llvm::ELF::R_386_32:
      // If buiding PIC object (shared library or PIC executable),
      // a dynamic relocations with RELATIVE type to this location is needed.
      // Reserve an entry in .rel.dyn
      if(Output::DynObj == pOutput.type()) {
        // create .rel.dyn section if not exist
        if(NULL == m_pRelDyn)
          createX86RelDyn(pLinker, pOutput);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        // set Rel bit
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      return;

    case llvm::ELF::R_386_GOTOFF:
    case llvm::ELF::R_386_GOTPC:
      // A GOT section is needed
      if(NULL == m_pGOT)
        createX86GOT(pLinker, pOutput);
      return;

    case llvm::ELF::R_386_PC32:
      return;

    default:
      llvm::report_fatal_error(llvm::Twine("unexpected reloc ") +
                               llvm::Twine((int) pReloc.type()) +
                               llvm::Twine(" in object file"));
      break;
  } // end switch
}

void X86GNULDBackend::scanGlobalReloc(Relocation& pReloc,
                                      const LDSymbol& pInputSym,
                                      MCLinker& pLinker,
                                      const MCLDInfo& pLDInfo,
                                      const Output& pOutput)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

  switch(pReloc.type()) {
    case llvm::ELF::R_386_32:
      // Absolute relocation type, symbol may needs PLT entry or
      // dynamic relocation entry
      if(isSymbolNeedsPLT(*rsym, pLDInfo, pOutput)) {
        // create plt for this symbol if it does not have one
        if(!(rsym->reserved() & ReservePLT)){
          // Create .got section if it dosen't exist
          if(NULL == m_pGOT)
             createX86GOT(pLinker, pOutput);
          // create .plt and .rel.plt if not exist
          if(NULL == m_pPLT)
            createX86PLTandRelPLT(pLinker, pOutput);
          // Symbol needs PLT entry, we need to reserve a PLT entry
          // and the corresponding GOT and dynamic relocation entry
          // in .got and .rel.plt. (GOT entry will be reserved simultaneously
          // when calling X86PLT->reserveEntry())
          m_pPLT->reserveEntry();
          m_pRelPLT->reserveEntry(*m_pRelocFactory);
          // set PLT bit
          rsym->setReserved(rsym->reserved() | ReservePLT);
        }
      }

      if(isSymbolNeedsDynRel(*rsym, pOutput, true)) {
        // symbol needs dynamic relocation entry, reserve an entry in .rel.dyn
        // create .rel.dyn section if not exist
        if(NULL == m_pRelDyn)
          createX86RelDyn(pLinker, pOutput);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        // set Rel bit
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      return;

    case llvm::ELF::R_386_GOTOFF:
    case llvm::ELF::R_386_GOTPC: {
      // A GOT section is needed
      if(NULL == m_pGOT)
        createX86GOT(pLinker, pOutput);
      return;
    }

    case llvm::ELF::R_386_PLT32:
      // A PLT entry is needed when building shared library

      // return if we already create plt for this symbol
      if(rsym->reserved() & ReservePLT)
        return;

      // if symbol is defined in the ouput file and it's not
      // preemptible, no need plt
      if(rsym->isDefine() && !rsym->isDyn() &&
         !isSymbolPreemptible(*rsym, pLDInfo, pOutput)) {
        return;
      }

      // Create .got section if it dosen't exist
      if(NULL == m_pGOT)
         createX86GOT(pLinker, pOutput);
      // create .plt and .rel.plt if not exist
      if(NULL == m_pPLT)
         createX86PLTandRelPLT(pLinker, pOutput);
      // Symbol needs PLT entry, we need to reserve a PLT entry
      // and the corresponding GOT and dynamic relocation entry
      // in .got and .rel.plt. (GOT entry will be reserved simultaneously
      // when calling X86PLT->reserveEntry())
      m_pPLT->reserveEntry();
      m_pRelPLT->reserveEntry(*m_pRelocFactory);
      // set PLT bit
      rsym->setReserved(rsym->reserved() | ReservePLT);
      return;

    case llvm::ELF::R_386_GOT32:
      // Symbol needs GOT entry, reserve entry in .got
      // return if we already create GOT for this symbol
      if(rsym->reserved() & (ReserveGOT | GOTRel))
        return;
      if(NULL == m_pGOT)
        createX86GOT(pLinker, pOutput);
      m_pGOT->reserveEntry();
      // If building shared object or the symbol is undefined, a dynamic
      // relocation is needed to relocate this GOT entry. Reserve an
      // entry in .rel.dyn
      if(Output::DynObj == pOutput.type() || rsym->isUndef() || rsym->isDyn()) {
        // create .rel.dyn section if not exist
        if(NULL == m_pRelDyn)
          createX86RelDyn(pLinker, pOutput);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        // set GOTRel bit
        rsym->setReserved(rsym->reserved() | GOTRel);
        return;
      }
      // set GOT bit
      rsym->setReserved(rsym->reserved() | ReserveGOT);
      return;

    case llvm::ELF::R_386_PC32:
      // We allow R_386_PC32 only if it isn't preemptible.  Otherwise
      // we will generate writable text section in output.
      if (!isSymbolPreemptible(*rsym, pLDInfo, pOutput))
	return;

    default: {
      llvm::report_fatal_error(llvm::Twine("Unexpected reloc ") +
                               llvm::Twine((int) pReloc.type()) +
                               llvm::Twine(" in object file"));
      break;
    }
  } // end switch
}

void X86GNULDBackend::scanRelocation(Relocation& pReloc,
                                     const LDSymbol& pInputSym,
                                     MCLinker& pLinker,
                                     const MCLDInfo& pLDInfo,
                                     const Output& pOutput)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  assert(NULL != rsym && "ResolveInfo of relocation not set while scanRelocation");

  // Scan relocation type to determine if an GOT/PLT/Dynamic Relocation
  // entries should be created.
  // FIXME: Below judgements concern only .so is generated as output
  // FIXME: Below judgements concren nothing about TLS related relocation

  // A refernece to symbol _GLOBAL_OFFSET_TABLE_ implies that a .got section
  // is needed
  if(NULL == m_pGOT && NULL != m_pGOTSymbol) {
    if(rsym == m_pGOTSymbol->resolveInfo()) {
      createX86GOT(pLinker, pOutput);
    }
  }

  // rsym is local
  if(rsym->isLocal())
    scanLocalReloc(pReloc, pInputSym,  pLinker, pLDInfo, pOutput);

  // rsym is external
  else
    scanGlobalReloc(pReloc, pInputSym ,pLinker, pLDInfo, pOutput);

}

uint64_t X86GNULDBackend::emitSectionData(const Output& pOutput,
                                          const LDSection& pSection,
                                          const MCLDInfo& pInfo,
                                          MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  ELFFileFormat* FileFormat = getOutputFormat(pOutput);
  assert(FileFormat &&
         "ELFFileFormat is NULL in X86GNULDBackend::emitSectionData!");

  unsigned int EntrySize = 0;
  uint64_t RegionSize = 0;

  if (&pSection == &(FileFormat->getPLT())) {
    assert(m_pPLT && "emitSectionData failed, m_pPLT is NULL!");

    unsigned char* buffer = pRegion.getBuffer();

    m_pPLT->applyPLT0();
    m_pPLT->applyPLT1();

    X86PLT::iterator it = m_pPLT->begin();
    unsigned int plt0_size = llvm::cast<X86PLT0>((*it)).getEntrySize();

    memcpy(buffer, llvm::cast<X86PLT0>((*it)).getContent(), plt0_size);
    RegionSize += plt0_size;
    ++it;

    X86PLT1* plt1 = 0;
    X86PLT::iterator ie = m_pPLT->end();
    while (it != ie) {
      plt1 = &(llvm::cast<X86PLT1>(*it));
      EntrySize = plt1->getEntrySize();
      memcpy(buffer + RegionSize, plt1->getContent(), EntrySize);
      RegionSize += EntrySize;
      ++it;
    }
  }

  else if (&pSection == &(FileFormat->getGOT())) {
    assert(m_pGOT && "emitSectionData failed, m_pGOT is NULL!");

    m_pGOT->applyGOT0(FileFormat->getDynamic().addr());

    uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

    GOTEntry* got = 0;
    EntrySize = m_pGOT->getEntrySize();

    for (X86GOT::iterator it = m_pGOT->begin(),
         ie = m_pGOT->end(); it != ie; ++it, ++buffer) {
      got = &(llvm::cast<GOTEntry>((*it)));
      *buffer = static_cast<uint32_t>(got->getContent());
      RegionSize += EntrySize;
    }
  }

  else
    llvm::report_fatal_error("unsupported section name "
                             + pSection.name() + " !");

  return RegionSize;
}
uint32_t X86GNULDBackend::machine() const
{
  return llvm::ELF::EM_386;
}

X86GOT& X86GNULDBackend::getGOT()
{
  assert(NULL != m_pGOT);
  return *m_pGOT;
}

const X86GOT& X86GNULDBackend::getGOT() const
{
  assert(NULL != m_pGOT);
  return *m_pGOT;
}

X86PLT& X86GNULDBackend::getPLT()
{
  assert(NULL != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

const X86PLT& X86GNULDBackend::getPLT() const
{
  assert(NULL != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

OutputRelocSection& X86GNULDBackend::getRelDyn()
{
  assert(NULL != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

const OutputRelocSection& X86GNULDBackend::getRelDyn() const
{
  assert(NULL != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

OutputRelocSection& X86GNULDBackend::getRelPLT()
{
  assert(NULL != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

const OutputRelocSection& X86GNULDBackend::getRelPLT() const
{
  assert(NULL != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

unsigned int
X86GNULDBackend::getTargetSectionOrder(const Output& pOutput,
                                       const LDSection& pSectHdr) const
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  // FIXME: if command line option, "-z now", is given, we can let the order of
  // .got and .got.plt be the same as RELRO sections
  if (&pSectHdr == &file_format->getGOT())
    return SHO_RELRO_LAST;

  if (&pSectHdr == &file_format->getGOTPLT())
    return SHO_NON_RELRO_FIRST;

  if (&pSectHdr == &file_format->getPLT())
    return SHO_PLT;

  return SHO_UNDEFINED;
}

unsigned int X86GNULDBackend::bitclass() const
{
  return 32;
}

bool X86GNULDBackend::initTargetSectionMap(SectionMap& pSectionMap)
{
  return true;
}

void X86GNULDBackend::initTargetSections(MCLinker& pLinker)
{
}

void X86GNULDBackend::initTargetSymbols(MCLinker& pLinker)
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

/// finalizeSymbol - finalize the symbol value
/// If the symbol's reserved field is not zero, MCLinker will call back this
/// function to ask the final value of the symbol
bool X86GNULDBackend::finalizeSymbol(LDSymbol& pSymbol) const
{
  return false;
}

/// allocateCommonSymbols - allocate common symbols in the corresponding
/// sections.
/// @refer Google gold linker: common.cc: 214
bool
X86GNULDBackend::allocateCommonSymbols(const MCLDInfo& pInfo, MCLinker& pLinker) const
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
      // We have to reset the description of the symbol here. When doing
      // incremental linking, the output relocatable object may have common
      // symbols. Therefore, we can not treat common symbols as normal symbols
      // when emitting the regular name pools. We must change the symbols'
      // description here.
      (*com_sym)->resolveInfo()->setDesc(ResolveInfo::Define);
      llvm::MCFragment* frag = new llvm::MCFillFragment(0x0, 1, (*com_sym)->size());
      (*com_sym)->setFragmentRef(new MCFragmentRef(*frag, 0));
      uint64_t size = pLinker.getLayout().appendFragment(*frag,
                                                         bss_section,
                                                         (*com_sym)->value());
      offset += size;
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
    llvm::MCFragment* frag = new llvm::MCFillFragment(0x0, 1, (*com_sym)->size());
    (*com_sym)->setFragmentRef(new MCFragmentRef(*frag, 0));
    uint64_t size = pLinker.getLayout().appendFragment(*frag,
                                                       bss_section,
                                                       (*com_sym)->value());
    offset += size;
  }

  bss_sect_hdr->setSize(offset);
  symbol_list.changeCommonsToGlobal();
  return true;
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createX86LDBackend - the help funtion to create corresponding X86LDBackend
///
TargetLDBackend* createX86LDBackend(const llvm::Target& pTarget,
                                    const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
    /**
    return new X86MachOLDBackend(createX86MachOArchiveReader,
                               createX86MachOObjectReader,
                               createX86MachOObjectWriter);
    **/
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
    /**
    return new X86COFFLDBackend(createX86COFFArchiveReader,
                               createX86COFFObjectReader,
                               createX86COFFObjectWriter);
    **/
  }
  return new X86GNULDBackend();
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeX86LDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheX86Target, createX86LDBackend);
}
