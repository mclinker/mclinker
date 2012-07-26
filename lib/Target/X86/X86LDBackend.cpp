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
#include <llvm/Support/Casting.h>

#include <mcld/LD/SectionMap.h>
#include <mcld/LD/FillFragment.h>
#include <mcld/LD/RegionFragment.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/TargetRegistry.h>

#include <cstring>

using namespace mcld;

X86GNULDBackend::X86GNULDBackend()
  : m_pRelocFactory(NULL),
    m_pGOT(NULL),
    m_pPLT(NULL),
    m_pGOTPLT(NULL),
    m_pRelDyn(NULL),
    m_pRelPLT(NULL),
    m_pDynamic(NULL),
    m_pGOTSymbol(NULL) {
}

X86GNULDBackend::~X86GNULDBackend()
{
  if (NULL != m_pRelocFactory)
    delete m_pRelocFactory;
  if (NULL != m_pGOT)
    delete m_pGOT;
  if (NULL != m_pPLT)
    delete m_pPLT;
  if (NULL != m_pGOTPLT)
    delete m_pGOTPLT;
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
  if (Output::DynObj == pOutput.type() && (NULL == m_pGOTPLT)) {
    createX86GOTPLT(pLinker, pOutput);
  }
}

void X86GNULDBackend::doPostLayout(const Output& pOutput,
                                   const MCLDInfo& pInfo,
                                   MCLinker& pLinker)
{
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
  // get .got LDSection and create SectionData
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& got = file_format->getGOT();
  m_pGOT = new X86GOT(got, pLinker.getOrCreateSectData(got));
}

void X86GNULDBackend::createX86GOTPLT(MCLinker& pLinker, const Output& pOutput)
{
  // get .got.plt LDSection and create SectionData
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& gotplt = file_format->getGOTPLT();
  m_pGOTPLT = new X86GOTPLT(gotplt, pLinker.getOrCreateSectData(gotplt));

  // define symbol _GLOBAL_OFFSET_TABLE_ when .got.plt create
  if (m_pGOTSymbol != NULL) {
    pLinker.defineSymbol<MCLinker::Force, MCLinker::Unresolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     false,
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     pLinker.getLayout().getFragmentRef(*(m_pGOTPLT->begin()),
                                                         0x0),
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
                     pLinker.getLayout().getFragmentRef(*(m_pGOTPLT->begin()),
                                                          0x0),
                     ResolveInfo::Hidden);
  }
}

void X86GNULDBackend::createX86PLTandRelPLT(MCLinker& pLinker,
                                            const Output& pOutput)
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& plt = file_format->getPLT();
  LDSection& relplt = file_format->getRelPlt();
  assert(m_pGOTPLT != NULL);
  // create SectionData and X86PLT
  m_pPLT = new X86PLT(plt, pLinker.getOrCreateSectData(plt), *m_pGOTPLT, pOutput);

  // set info of .rel.plt to .plt
  relplt.setLink(&plt);
  // create SectionData and X86RelDynSection
  m_pRelPLT = new OutputRelocSection(relplt,
                                     pLinker.getOrCreateSectData(relplt),
                                     8);
}

void X86GNULDBackend::createX86RelDyn(MCLinker& pLinker,
                                      const Output& pOutput)
{
  // get .rel.dyn LDSection and create SectionData
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  LDSection& reldyn = file_format->getRelDyn();
  // create SectionData and X86RelDynSection
  m_pRelDyn = new OutputRelocSection(reldyn,
                                     pLinker.getOrCreateSectData(reldyn),
                                     8);
}

void X86GNULDBackend::addCopyReloc(ResolveInfo& pSym)
{
  bool exist;
  Relocation& rel_entry = *m_pRelDyn->getEntry(pSym, false, exist);
  rel_entry.setType(llvm::ELF::R_386_COPY);
  assert(pSym.outSymbol()->hasFragRef());
  rel_entry.targetRef().assign(*pSym.outSymbol()->fragRef());
  rel_entry.setSymInfo(&pSym);
}

LDSymbol& X86GNULDBackend::defineSymbolforCopyReloc(MCLinker& pLinker,
                                                    const ResolveInfo& pSym)
{
  // For a symbol needing copy relocation, define a copy symbol in the BSS
  // section and all other reference to this symbol should refer to this
  // copy.

  // get or create corresponding BSS LDSection
  LDSection* bss_sect_hdr = NULL;
  if (ResolveInfo::ThreadLocal == pSym.type()) {
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

  // get or create corresponding BSS SectionData
  assert(NULL != bss_sect_hdr);
  SectionData& bss_section = pLinker.getOrCreateSectData(
                                     *bss_sect_hdr);

  // Determine the alignment by the symbol value
  // FIXME: here we use the largest alignment
  uint32_t addralign = bitclass() / 8;

  // allocate space in BSS for the copy symbol
  Fragment* frag = new FillFragment(0x0, 1, pSym.size());
  uint64_t size = pLinker.getLayout().appendFragment(*frag,
                                                     bss_section,
                                                     addralign);
  bss_sect_hdr->setSize(bss_sect_hdr->size() + size);

  // change symbol binding to Global if it's a weak symbol
  ResolveInfo::Binding binding = (ResolveInfo::Binding)pSym.binding();
  if (binding == ResolveInfo::Weak)
    binding = ResolveInfo::Global;

  // Define the copy symbol in the bss section and resolve it
  LDSymbol* cpy_sym = pLinker.defineSymbol<MCLinker::Force, MCLinker::Resolve>(
                      pSym.name(),
                      false,
                      (ResolveInfo::Type)pSym.type(),
                      ResolveInfo::Define,
                      binding,
                      pSym.size(),  // size
                      0x0,          // value
                      pLinker.getLayout().getFragmentRef(*frag, 0x0),
                      (ResolveInfo::Visibility)pSym.other());

  return *cpy_sym;
}

void X86GNULDBackend::updateAddend(Relocation& pReloc,
                                   const LDSymbol& pInputSym,
                                   const Layout& pLayout) const
{
  // Update value keep in addend if we meet a section symbol
  if (pReloc.symInfo()->type() == ResolveInfo::Section) {
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
      if (isOutputPIC(pOutput, pLDInfo)) {
        // create .rel.dyn section if not exist
        if (NULL == m_pRelDyn)
          createX86RelDyn(pLinker, pOutput);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        // set Rel bit
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      return;

    case llvm::ELF::R_386_GOTOFF:
    case llvm::ELF::R_386_GOTPC:
      // A GOT section is needed
      if (NULL == m_pGOT)
        createX86GOT(pLinker, pOutput);
      return;

    case llvm::ELF::R_386_PC32:
      return;

    default:
      fatal(diag::unsupported_relocation) << (int)pReloc.type()
                                          << "mclinker@googlegroups.com";
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
      if (symbolNeedsPLT(*rsym, pLDInfo, pOutput)) {
        // create plt for this symbol if it does not have one
        if (!(rsym->reserved() & ReservePLT)){
          // Create .got section if it dosen't exist
          if (NULL == m_pGOTPLT)
            createX86GOTPLT(pLinker, pOutput);
          // create .plt and .rel.plt if not exist
          if (NULL == m_pPLT)
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

      if (symbolNeedsDynRel(*rsym, (rsym->reserved() & ReservePLT),
                            pLDInfo, pOutput, true)) {
        // symbol needs dynamic relocation entry, reserve an entry in .rel.dyn
        // create .rel.dyn section if not exist
        if (NULL == m_pRelDyn)
          createX86RelDyn(pLinker, pOutput);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        if (symbolNeedsCopyReloc(pLinker.getLayout(), pReloc, *rsym, pLDInfo,
                          pOutput)) {
          LDSymbol& cpy_sym = defineSymbolforCopyReloc(pLinker, *rsym);
          addCopyReloc(*cpy_sym.resolveInfo());
        }
        else {
          // set Rel bit
          rsym->setReserved(rsym->reserved() | ReserveRel);
        }
      }
      return;

    case llvm::ELF::R_386_GOTOFF:
    case llvm::ELF::R_386_GOTPC: {
      // A GOT section is needed
      if (NULL == m_pGOT)
        createX86GOT(pLinker, pOutput);
      return;
    }

    case llvm::ELF::R_386_PLT32:
      // A PLT entry is needed when building shared library

      // return if we already create plt for this symbol
      if (rsym->reserved() & ReservePLT)
        return;

      // if symbol is defined in the ouput file and it's not
      // preemptible, no need plt
      if (rsym->isDefine() && !rsym->isDyn() &&
         !isSymbolPreemptible(*rsym, pLDInfo, pOutput)) {
        return;
      }

      // Create .got section if it dosen't exist
      if (NULL == m_pGOTPLT)
         createX86GOTPLT(pLinker, pOutput);
      // create .plt and .rel.plt if not exist
      if (NULL == m_pPLT)
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
      if (rsym->reserved() & (ReserveGOT | GOTRel))
        return;
      if (NULL == m_pGOT)
        createX86GOT(pLinker, pOutput);
      m_pGOT->reserveEntry();
      // If building shared object or the symbol is undefined, a dynamic
      // relocation is needed to relocate this GOT entry. Reserve an
      // entry in .rel.dyn
      if (Output::DynObj == pOutput.type() || rsym->isUndef() || rsym->isDyn()) {
        // create .rel.dyn section if not exist
        if (NULL == m_pRelDyn)
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

      if (symbolNeedsPLT(*rsym, pLDInfo, pOutput) &&
          pOutput.type() != Output::DynObj) {
        // create plt for this symbol if it does not have one
        if (!(rsym->reserved() & ReservePLT)){
          // Create .got section if it dosen't exist
          if (NULL == m_pGOTPLT)
            createX86GOTPLT(pLinker, pOutput);
          // create .plt and .rel.plt if not exist
          if (NULL == m_pPLT)
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

      if (symbolNeedsDynRel(*rsym, (rsym->reserved() & ReservePLT),
                            pLDInfo, pOutput, false)) {
        // symbol needs dynamic relocation entry, reserve an entry in .rel.dyn
        // create .rel.dyn section if not exist
        if (NULL == m_pRelDyn)
          createX86RelDyn(pLinker, pOutput);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        if (symbolNeedsCopyReloc(pLinker.getLayout(), pReloc, *rsym, pLDInfo,
                          pOutput)) {
          LDSymbol& cpy_sym = defineSymbolforCopyReloc(pLinker, *rsym);
          addCopyReloc(*cpy_sym.resolveInfo());
        }
        else {
          // set Rel bit
          rsym->setReserved(rsym->reserved() | ReserveRel);
        }
      }
      return;
    default: {
      fatal(diag::unsupported_relocation) << (int)pReloc.type()
                                          << "mclinker@googlegroups.com";
      break;
    }
  } // end switch
}

void X86GNULDBackend::scanRelocation(Relocation& pReloc,
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

  // Scan relocation type to determine if an GOT/PLT/Dynamic Relocation
  // entries should be created.
  // FIXME: Below judgements concern only .so is generated as output
  // FIXME: Below judgements concren nothing about TLS related relocation

  // A refernece to symbol _GLOBAL_OFFSET_TABLE_ implies that a .got.plt
  // section is needed
  if (NULL == m_pGOTPLT && NULL != m_pGOTSymbol) {
    if (rsym == m_pGOTSymbol->resolveInfo()) {
      createX86GOTPLT(pLinker, pOutput);
    }
  }

  // rsym is local
  if (rsym->isLocal())
    scanLocalReloc(pReloc, pInputSym,  pLinker, pLDInfo, pOutput);

  // rsym is external
  else
    scanGlobalReloc(pReloc, pInputSym ,pLinker, pLDInfo, pOutput);

}

uint64_t X86GNULDBackend::emitSectionData(const Output& pOutput,
                                          const LDSection& pSection,
                                          const MCLDInfo& pInfo,
                                          const Layout& pLayout,
                                          MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  const ELFFileFormat* FileFormat = getOutputFormat(pOutput);
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

  else if (&pSection == &(FileFormat->getGOTPLT())) {
    assert(m_pGOTPLT && "emitSectionData failed, m_pGOTPLT is NULL!");
    m_pGOTPLT->applyGOT0(FileFormat->getDynamic().addr());

    uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

    GOTEntry* got = 0;
    EntrySize = m_pGOTPLT->getEntrySize();

    for (X86GOTPLT::iterator it = m_pGOTPLT->begin(),
         ie = m_pGOTPLT->end(); it != ie; ++it, ++buffer) {
      got = &(llvm::cast<GOTEntry>((*it)));
      *buffer = static_cast<uint32_t>(got->getContent());
      RegionSize += EntrySize;
    }
  }

  else {
    fatal(diag::unrecognized_output_sectoin)
            << pSection.name()
            << "mclinker@googlegroups.com";
  }
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

X86GOTPLT& X86GNULDBackend::getGOTPLT()
{
  assert(NULL != m_pGOTPLT);
  return *m_pGOTPLT;
}

const X86GOTPLT& X86GNULDBackend::getGOTPLT() const
{
  assert(NULL != m_pGOTPLT);
  return *m_pGOTPLT;
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
                                       const LDSection& pSectHdr,
                                       const MCLDInfo& pInfo) const
{
  const ELFFileFormat* file_format = getOutputFormat(pOutput);

  if (&pSectHdr == &file_format->getGOT()) {
    if (pInfo.options().hasNow())
      return SHO_RELRO;
    return SHO_RELRO_LAST;
  }

  if (&pSectHdr == &file_format->getGOTPLT()) {
    if (pInfo.options().hasNow())
      return SHO_RELRO;
    return SHO_NON_RELRO_FIRST;
  }

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

void X86GNULDBackend::initTargetSymbols(MCLinker& pLinker, const Output& pOutput)
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
bool X86GNULDBackend::finalizeTargetSymbols(MCLinker& pLinker, const Output& pOutput)
{
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
