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
#include "X86Relocator.h"
#include "X86GNUInfo.h"

#include <llvm/ADT/Triple.h>
#include <llvm/Support/Casting.h>

#include <mcld/LinkerConfig.h>
#include <mcld/IRBuilder.h>
#include <mcld/Fragment/FillFragment.h>
#include <mcld/Fragment/RegionFragment.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Object/ObjectBuilder.h>

#include <cstring>

using namespace mcld;

//===----------------------------------------------------------------------===//
// X86GNULDBackend
//===----------------------------------------------------------------------===//
X86GNULDBackend::X86GNULDBackend(const LinkerConfig& pConfig, X86GNUInfo* pInfo)
  : GNULDBackend(pConfig, pInfo),
    m_pRelocator(NULL),
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
  delete m_pRelocator;
  delete m_pGOT;
  delete m_pPLT;
  delete m_pGOTPLT;
  delete m_pRelDyn;
  delete m_pRelPLT;
  delete m_pDynamic;
}

bool X86GNULDBackend::initRelocator(const FragmentLinker& pLinker)
{
  if (NULL == m_pRelocator) {
    m_pRelocator = new X86Relocator(*this);
    m_pRelocator->setFragmentLinker(pLinker);
  }
  return true;
}

Relocator* X86GNULDBackend::getRelocator()
{
  assert(NULL != m_pRelocator);
  return m_pRelocator;
}

void X86GNULDBackend::doPreLayout(FragmentLinker& pLinker)
{
  // set .got.plt size
  // when building shared object, the .got section is must
  if (LinkerConfig::Object != config().codeGenType()) {
    if (LinkerConfig::DynObj == config().codeGenType() ||
        m_pGOTPLT->hasGOT1() ||
        NULL != m_pGOTSymbol) {
      m_pGOTPLT->finalizeSectionSize();
      defineGOTSymbol(pLinker);
    }

    // set .got size
    if (!m_pGOT->empty())
      m_pGOT->finalizeSectionSize();

    // set .plt size
    if (m_pPLT->hasPLT1())
      m_pPLT->finalizeSectionSize();

    ELFFileFormat* file_format = getOutputFormat();
    // set .rel.dyn size
    if (!m_pRelDyn->empty())
      file_format->getRelDyn().setSize(
                                  m_pRelDyn->numOfRelocs() * getRelEntrySize());

    // set .rel.plt size
    if (!m_pRelPLT->empty())
      file_format->getRelPlt().setSize(
                                  m_pRelPLT->numOfRelocs() * getRelEntrySize());
  }
}

void X86GNULDBackend::doPostLayout(Module& pModule,
                                   FragmentLinker& pLinker)
{
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
X86ELFDynamic& X86GNULDBackend::dynamic()
{
  if (NULL == m_pDynamic)
    m_pDynamic = new X86ELFDynamic(*this, config());

  return *m_pDynamic;
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
const X86ELFDynamic& X86GNULDBackend::dynamic() const
{
  assert( NULL != m_pDynamic);
  return *m_pDynamic;
}

void X86GNULDBackend::defineGOTSymbol(FragmentLinker& pLinker)
{
  // define symbol _GLOBAL_OFFSET_TABLE_
  if (m_pGOTSymbol != NULL) {
    pLinker.defineSymbol<FragmentLinker::Force, FragmentLinker::Unresolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     false,
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     FragmentRef::Create(*(m_pGOTPLT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
  else {
    m_pGOTSymbol = pLinker.defineSymbol<FragmentLinker::Force, FragmentLinker::Resolve>(
                     "_GLOBAL_OFFSET_TABLE_",
                     false,
                     ResolveInfo::Object,
                     ResolveInfo::Define,
                     ResolveInfo::Local,
                     0x0, // size
                     0x0, // value
                     FragmentRef::Create(*(m_pGOTPLT->begin()), 0x0),
                     ResolveInfo::Hidden);
  }
}

void X86GNULDBackend::addCopyReloc(ResolveInfo& pSym)
{
  Relocation& rel_entry = *m_pRelDyn->consumeEntry();
  rel_entry.setType(llvm::ELF::R_386_COPY);
  assert(pSym.outSymbol()->hasFragRef());
  rel_entry.targetRef().assign(*pSym.outSymbol()->fragRef());
  rel_entry.setSymInfo(&pSym);
}

/// defineSymbolforCopyReloc
/// For a symbol needing copy relocation, define a copy symbol in the BSS
/// section and all other reference to this symbol should refer to this
/// copy.
/// @note This is executed at `scan relocation' stage.
LDSymbol& X86GNULDBackend::defineSymbolforCopyReloc(FragmentLinker& pLinker,
                                                    const ResolveInfo& pSym)
{
  // get or create corresponding BSS LDSection
  LDSection* bss_sect_hdr = NULL;
  ELFFileFormat* file_format = getOutputFormat();
  if (ResolveInfo::ThreadLocal == pSym.type())
    bss_sect_hdr = &file_format->getTBSS();
  else
    bss_sect_hdr = &file_format->getBSS();

  // get or create corresponding BSS SectionData
  assert(NULL != bss_sect_hdr);
  SectionData* bss_section = NULL;
  if (bss_sect_hdr->hasSectionData())
    bss_section = bss_sect_hdr->getSectionData();
  else
    bss_section = IRBuilder::CreateSectionData(*bss_sect_hdr);

  // Determine the alignment by the symbol value
  // FIXME: here we use the largest alignment
  uint32_t addralign = config().targets().bitclass() / 8;

  // allocate space in BSS for the copy symbol
  Fragment* frag = new FillFragment(0x0, 1, pSym.size());
  uint64_t size = ObjectBuilder::AppendFragment(*frag,
                                                *bss_section,
                                                addralign);
  bss_sect_hdr->setSize(bss_sect_hdr->size() + size);

  // change symbol binding to Global if it's a weak symbol
  ResolveInfo::Binding binding = (ResolveInfo::Binding)pSym.binding();
  if (binding == ResolveInfo::Weak)
    binding = ResolveInfo::Global;

  // Define the copy symbol in the bss section and resolve it
  LDSymbol* cpy_sym =
           pLinker.defineSymbol<FragmentLinker::Force, FragmentLinker::Resolve>(
                      pSym.name(),
                      false,
                      (ResolveInfo::Type)pSym.type(),
                      ResolveInfo::Define,
                      binding,
                      pSym.size(),  // size
                      0x0,          // value
                      FragmentRef::Create(*frag, 0x0),
                      (ResolveInfo::Visibility)pSym.other());

  return *cpy_sym;
}

void X86GNULDBackend::scanLocalReloc(Relocation& pReloc,
                                     FragmentLinker& pLinker,
                                     Module& pModule,
                                     const LDSection& pSection)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

  switch(pReloc.type()){

    case llvm::ELF::R_386_32:
    case llvm::ELF::R_386_16:
    case llvm::ELF::R_386_8:
      // If buiding PIC object (shared library or PIC executable),
      // a dynamic relocations with RELATIVE type to this location is needed.
      // Reserve an entry in .rel.dyn
      if (pLinker.isOutputPIC()) {
        m_pRelDyn->reserveEntry();
        // set Rel bit
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      return;

    case llvm::ELF::R_386_GOTOFF:
    case llvm::ELF::R_386_GOTPC:
      // FIXME: A GOT section is needed
      return;

    case llvm::ELF::R_386_GOT32:
      // Symbol needs GOT entry, reserve entry in .got
      // return if we already create GOT for this symbol
      if (rsym->reserved() & (ReserveGOT | GOTRel))
        return;
      // FIXME: check STT_GNU_IFUNC symbol
      m_pGOT->reserve();
      // If building shared object or the symbol is undefined, a dynamic
      // relocation is needed to relocate this GOT entry. Reserve an
      // entry in .rel.dyn
      if (LinkerConfig::DynObj ==
                   config().codeGenType() || rsym->isUndef() || rsym->isDyn()) {
        m_pRelDyn->reserveEntry();
        // set GOTRel bit
        rsym->setReserved(rsym->reserved() | GOTRel);
        return;
      }
      // set GOT bit
      rsym->setReserved(rsym->reserved() | ReserveGOT);
      return;

    case llvm::ELF::R_386_PC32:
    case llvm::ELF::R_386_PC16:
    case llvm::ELF::R_386_PC8:
      return;

    case llvm::ELF::R_386_TLS_GD: {
      // FIXME: no linker optimization for TLS relocation
      if (rsym->reserved() & GOTRel)
        return;
      m_pGOT->reserve(2);
      // reserve an rel entry
      m_pRelDyn->reserveEntry();
      // set GOTRel bit
      rsym->setReserved(rsym->reserved() | GOTRel);
      // define the section symbol for .tdata or .tbss
      // the target symbol of the created dynamic relocation should be the
      // section symbol of the section which this symbol defined. so we
      // need to define that section symbol here
      ELFFileFormat* file_format = getOutputFormat();
      const LDSection* sym_sect =
               &rsym->outSymbol()->fragRef()->frag()->getParent()->getSection();
      if (&file_format->getTData() == sym_sect) {
        if (NULL == f_pTDATA)
          f_pTDATA = pModule.getSectionSymbolSet().get(*sym_sect);
      }
      else if (&file_format->getTBSS() == sym_sect || rsym->isCommon()) {
        if (NULL == f_pTBSS)
          f_pTBSS = pModule.getSectionSymbolSet().get(*sym_sect);
      }
      else
        error(diag::invalid_tls) << rsym->name() << sym_sect->name();
      return;
    }

    case llvm::ELF::R_386_TLS_LDM:
      getTLSModuleID();
      return;

    case llvm::ELF::R_386_TLS_LDO_32:
      return;

    case llvm::ELF::R_386_TLS_IE:
      setHasStaticTLS();
      // if buildint shared object, a RELATIVE dynamic relocation is needed
      if (LinkerConfig::DynObj == config().codeGenType()) {
        m_pRelDyn->reserveEntry();
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      if (rsym->reserved() & GOTRel)
        return;
      // reserve got and dyn relocation entries for tp-relative offset
      m_pGOT->reserve();
      m_pRelDyn->reserveEntry();
      // set GOTRel bit
      rsym->setReserved(rsym->reserved() | GOTRel);
      m_pRelDyn->addSymbolToDynSym(*rsym->outSymbol());
      return;

    case llvm::ELF::R_386_TLS_GOTIE:
      setHasStaticTLS();
      if (rsym->reserved() & GOTRel)
        return;
      // reserve got and dyn relocation entries for tp-relative offset
      m_pGOT->reserve();
      m_pRelDyn->reserveEntry();
      // set GOTRel bit
      rsym->setReserved(rsym->reserved() | GOTRel);
      m_pRelDyn->addSymbolToDynSym(*rsym->outSymbol());
      return;

    case llvm::ELF::R_386_TLS_LE:
    case llvm::ELF::R_386_TLS_LE_32:
      setHasStaticTLS();
      // if buildint shared object, a dynamic relocation is needed
      if (LinkerConfig::DynObj == config().codeGenType()) {
        m_pRelDyn->reserveEntry();
        rsym->setReserved(rsym->reserved() | ReserveRel);
        // the target symbol of the dynamic relocation is rsym, so we need to
        // emit it into .dynsym
        assert(NULL != rsym->outSymbol());
        m_pRelDyn->addSymbolToDynSym(*rsym->outSymbol());
      }
      return;

    default:
      fatal(diag::unsupported_relocation) << (int)pReloc.type()
                                          << "mclinker@googlegroups.com";
      break;
  } // end switch
}

void X86GNULDBackend::scanGlobalReloc(Relocation& pReloc,
                                      FragmentLinker& pLinker,
                                      Module& pModule,
                                      const LDSection& pSection)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

  switch(pReloc.type()) {
    case llvm::ELF::R_386_32:
    case llvm::ELF::R_386_16:
    case llvm::ELF::R_386_8:
      // Absolute relocation type, symbol may needs PLT entry or
      // dynamic relocation entry
      if (symbolNeedsPLT(pLinker, *rsym)) {
        // create plt for this symbol if it does not have one
        if (!(rsym->reserved() & ReservePLT)){
          // Symbol needs PLT entry, we need to reserve a PLT entry
          // and the corresponding GOT and dynamic relocation entry
          // in .got and .rel.plt. (GOT entry will be reserved simultaneously
          // when calling X86PLT->reserveEntry())
          m_pPLT->reserveEntry();
          m_pGOTPLT->reserve();
          m_pRelPLT->reserveEntry();
          // set PLT bit
          rsym->setReserved(rsym->reserved() | ReservePLT);
        }
      }

      if (symbolNeedsDynRel(pLinker, *rsym, (rsym->reserved() & ReservePLT),
                                                                       true)) {
        // symbol needs dynamic relocation entry, reserve an entry in .rel.dyn
        m_pRelDyn->reserveEntry();
        if (symbolNeedsCopyReloc(pLinker, pReloc, *rsym)) {
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
      // FIXME: A GOT section is needed
      return;
    }

    case llvm::ELF::R_386_PLT32:
      // A PLT entry is needed when building shared library

      // return if we already create plt for this symbol
      if (rsym->reserved() & ReservePLT)
        return;

      // if the symbol's value can be decided at link time, then no need plt
      if (symbolFinalValueIsKnown(pLinker, *rsym))
        return;

      // if symbol is defined in the ouput file and it's not
      // preemptible, no need plt
      if (rsym->isDefine() && !rsym->isDyn() &&
         !isSymbolPreemptible(*rsym)) {
        return;
      }

      // Symbol needs PLT entry, we need to reserve a PLT entry
      // and the corresponding GOT and dynamic relocation entry
      // in .got and .rel.plt. (GOT entry will be reserved simultaneously
      // when calling X86PLT->reserveEntry())
      m_pPLT->reserveEntry();
      m_pGOTPLT->reserve();
      m_pRelPLT->reserveEntry();
      // set PLT bit
      rsym->setReserved(rsym->reserved() | ReservePLT);
      return;

    case llvm::ELF::R_386_GOT32:
      // Symbol needs GOT entry, reserve entry in .got
      // return if we already create GOT for this symbol
      if (rsym->reserved() & (ReserveGOT | GOTRel))
        return;
      m_pGOT->reserve();
      // If building shared object or the symbol is undefined, a dynamic
      // relocation is needed to relocate this GOT entry. Reserve an
      // entry in .rel.dyn
      if (LinkerConfig::DynObj ==
                   config().codeGenType() || rsym->isUndef() || rsym->isDyn()) {
        m_pRelDyn->reserveEntry();
        // set GOTRel bit
        rsym->setReserved(rsym->reserved() | GOTRel);
        return;
      }
      // set GOT bit
      rsym->setReserved(rsym->reserved() | ReserveGOT);
      return;

    case llvm::ELF::R_386_PC32:
    case llvm::ELF::R_386_PC16:
    case llvm::ELF::R_386_PC8:

      if (symbolNeedsPLT(pLinker, *rsym) &&
                               LinkerConfig::DynObj != config().codeGenType()) {
        // create plt for this symbol if it does not have one
        if (!(rsym->reserved() & ReservePLT)){
          // Symbol needs PLT entry, we need to reserve a PLT entry
          // and the corresponding GOT and dynamic relocation entry
          // in .got and .rel.plt. (GOT entry will be reserved simultaneously
          // when calling X86PLT->reserveEntry())
          m_pPLT->reserveEntry();
          m_pGOTPLT->reserve();
          m_pRelPLT->reserveEntry();
          // set PLT bit
          rsym->setReserved(rsym->reserved() | ReservePLT);
        }
      }

      if (symbolNeedsDynRel(pLinker, *rsym, (rsym->reserved() & ReservePLT),
                                                                      false)) {
        // symbol needs dynamic relocation entry, reserve an entry in .rel.dyn
        m_pRelDyn->reserveEntry();
        if (symbolNeedsCopyReloc(pLinker, pReloc, *rsym)) {
          LDSymbol& cpy_sym = defineSymbolforCopyReloc(pLinker, *rsym);
          addCopyReloc(*cpy_sym.resolveInfo());
        }
        else {
          // set Rel bit
          rsym->setReserved(rsym->reserved() | ReserveRel);
        }
      }
      return;

    case llvm::ELF::R_386_TLS_GD: {
      // FIXME: no linker optimization for TLS relocation
      if (rsym->reserved() & GOTRel)
        return;
      // reserve two pairs of got entry and dynamic relocation
      m_pGOT->reserve(2);
      m_pRelDyn->reserveEntry(2);
      // set GOTRel bit
      rsym->setReserved(rsym->reserved() | GOTRel);
      return;
    }

    case llvm::ELF::R_386_TLS_LDM:
      getTLSModuleID();
      return;

    case llvm::ELF::R_386_TLS_LDO_32:
      return;

    case llvm::ELF::R_386_TLS_IE:
      setHasStaticTLS();
      // if buildint shared object, a RELATIVE dynamic relocation is needed
      if (LinkerConfig::DynObj == config().codeGenType()) {
        m_pRelDyn->reserveEntry();
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      if (rsym->reserved() & GOTRel)
        return;
      // reserve got and dyn relocation entries for tp-relative offset
      m_pGOT->reserve();
      m_pRelDyn->reserveEntry();
      // set GOTRel bit
      rsym->setReserved(rsym->reserved() | GOTRel);
      return;

    case llvm::ELF::R_386_TLS_GOTIE:
      setHasStaticTLS();
      if (rsym->reserved() & GOTRel)
        return;
      // reserve got and dyn relocation entries for tp-relative offset
      m_pGOT->reserve();
      m_pRelDyn->reserveEntry();
      // set GOTRel bit
      rsym->setReserved(rsym->reserved() | GOTRel);
      return;

    case llvm::ELF::R_386_TLS_LE:
    case llvm::ELF::R_386_TLS_LE_32:
      setHasStaticTLS();
      // if buildint shared object, a dynamic relocation is needed
      if (LinkerConfig::DynObj == config().codeGenType()) {
        m_pRelDyn->reserveEntry();
        rsym->setReserved(rsym->reserved() | ReserveRel);
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
                                     FragmentLinker& pLinker,
                                     Module& pModule,
                                     const LDSection& pSection)
{
  if (LinkerConfig::Object == config().codeGenType())
    return;
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  assert(NULL != rsym &&
         "ResolveInfo of relocation not set while scanRelocation");

  pReloc.updateAddend();
  if (0 == (pSection.flag() & llvm::ELF::SHF_ALLOC))
    return;

  // Scan relocation type to determine if the GOT/PLT/Dynamic Relocation
  // entries should be created.
  if (rsym->isLocal()) // rsym is local
    scanLocalReloc(pReloc, pLinker, pModule, pSection);
  else // rsym is external
    scanGlobalReloc(pReloc, pLinker, pModule, pSection);

  // check if we shoule issue undefined reference for the relocation target
  // symbol
  if (rsym->isUndef() && !rsym->isDyn() && !rsym->isWeak() && !rsym->isNull())
    fatal(diag::undefined_reference) << rsym->name();

  if ((rsym->reserved() & ReserveRel) != 0x0) {
    // set hasTextRelSection if needed
    checkAndSetHasTextRel(pSection);
  }
}

uint64_t X86GNULDBackend::emitSectionData(const LDSection& pSection,
                                          MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  const ELFFileFormat* FileFormat = getOutputFormat();
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
    unsigned int plt0_size = llvm::cast<PLTEntryBase>((*it)).size();

    memcpy(buffer, llvm::cast<PLTEntryBase>((*it)).getValue(), plt0_size);
    RegionSize += plt0_size;
    ++it;

    PLTEntryBase* plt1 = 0;
    X86PLT::iterator ie = m_pPLT->end();
    while (it != ie) {
      plt1 = &(llvm::cast<PLTEntryBase>(*it));
      EntrySize = plt1->size();
      memcpy(buffer + RegionSize, plt1->getValue(), EntrySize);
      RegionSize += EntrySize;
      ++it;
    }
  }

  else if (&pSection == &(FileFormat->getGOT())) {
    assert(m_pGOT && "emitSectionData failed, m_pGOT is NULL!");

    uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

    X86GOTEntry* got = 0;
    EntrySize = X86GOTEntry::EntrySize;

    for (X86GOT::iterator it = m_pGOT->begin(),
         ie = m_pGOT->end(); it != ie; ++it, ++buffer) {
      got = &(llvm::cast<X86GOTEntry>((*it)));
      *buffer = static_cast<uint32_t>(got->getValue());
      RegionSize += EntrySize;
    }
  }

  else if (&pSection == &(FileFormat->getGOTPLT())) {
    assert(m_pGOTPLT && "emitSectionData failed, m_pGOTPLT is NULL!");
    m_pGOTPLT->applyGOT0(FileFormat->getDynamic().addr());
    m_pGOTPLT->applyAllGOTPLT(*m_pPLT);

    uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

    X86GOTEntry* got = 0;
    EntrySize = X86GOTPLTEntry::EntrySize;

    for (X86GOTPLT::iterator it = m_pGOTPLT->begin(),
         ie = m_pGOTPLT->end(); it != ie; ++it, ++buffer) {
      got = &(llvm::cast<X86GOTEntry>((*it)));
      *buffer = static_cast<uint32_t>(got->getValue());
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

// Create a GOT entry for the TLS module index
X86GOTEntry& X86GNULDBackend::getTLSModuleID()
{
  static X86GOTEntry* got_entry = NULL;
  if (NULL != got_entry)
    return *got_entry;

  // Allocate 2 got entries and 1 dynamic reloc for R_386_TLS_LDM
  m_pGOT->reserve(2);
  got_entry = m_pGOT->consume();
  m_pGOT->consume()->setValue(0x0);

  m_pRelDyn->reserveEntry();
  Relocation* rel_entry = m_pRelDyn->consumeEntry();
  rel_entry->setType(llvm::ELF::R_386_TLS_DTPMOD32);
  rel_entry->targetRef().assign(*got_entry, 0x0);
  rel_entry->setSymInfo(NULL);

  return *got_entry;
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
X86GNULDBackend::getTargetSectionOrder(const LDSection& pSectHdr) const
{
  const ELFFileFormat* file_format = getOutputFormat();

  if (&pSectHdr == &file_format->getGOT()) {
    if (config().options().hasNow())
      return SHO_RELRO;
    return SHO_RELRO_LAST;
  }

  if (&pSectHdr == &file_format->getGOTPLT()) {
    if (config().options().hasNow())
      return SHO_RELRO;
    return SHO_NON_RELRO_FIRST;
  }

  if (&pSectHdr == &file_format->getPLT())
    return SHO_PLT;

  return SHO_UNDEFINED;
}

void X86GNULDBackend::initTargetSections(Module& pModule, ObjectBuilder& pBuilder)
{
  if (LinkerConfig::Object != config().codeGenType()) {
    ELFFileFormat* file_format = getOutputFormat();
    // initialize .got
    LDSection& got = file_format->getGOT();
    m_pGOT = new X86GOT(got);

    // initialize .got.plt
    LDSection& gotplt = file_format->getGOTPLT();
    m_pGOTPLT = new X86GOTPLT(gotplt);

    // initialize .plt
    LDSection& plt = file_format->getPLT();
    m_pPLT = new X86PLT(plt,
                        *m_pGOTPLT,
                        config());

    // initialize .rel.plt
    LDSection& relplt = file_format->getRelPlt();
    relplt.setLink(&plt);
    m_pRelPLT = new OutputRelocSection(pModule, relplt);

    // initialize .rel.dyn
    LDSection& reldyn = file_format->getRelDyn();
    m_pRelDyn = new OutputRelocSection(pModule, reldyn);

  }
}

void X86GNULDBackend::initTargetSymbols(FragmentLinker& pLinker)
{
  if (LinkerConfig::Object != config().codeGenType()) {
    // Define the symbol _GLOBAL_OFFSET_TABLE_ if there is a symbol with the
    // same name in input
    m_pGOTSymbol =
      pLinker.defineSymbol<FragmentLinker::AsRefered,
                           FragmentLinker::Resolve>("_GLOBAL_OFFSET_TABLE_",
                                                    false,
                                                    ResolveInfo::Object,
                                                    ResolveInfo::Define,
                                                    ResolveInfo::Local,
                                                    0x0,  // size
                                                    0x0,  // value
                                                    FragmentRef::Null(), // FragRef
                                                    ResolveInfo::Hidden);
  }
}

/// finalizeSymbol - finalize the symbol value
bool X86GNULDBackend::finalizeTargetSymbols(FragmentLinker& pLinker)
{
  return true;
}

/// doCreateProgramHdrs - backend can implement this function to create the
/// target-dependent segments
void X86GNULDBackend::doCreateProgramHdrs(Module& pModule,
                                          const FragmentLinker& pLinker)
{
  // TODO
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createX86LDBackend - the help funtion to create corresponding X86LDBackend
///
TargetLDBackend* createX86LDBackend(const llvm::Target& pTarget,
                                    const LinkerConfig& pConfig)
{
  if (pConfig.targets().triple().isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
    /**
    return new X86MachOLDBackend(createX86MachOArchiveReader,
                               createX86MachOObjectReader,
                               createX86MachOObjectWriter);
    **/
  }
  if (pConfig.targets().triple().isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
    /**
    return new X86COFFLDBackend(createX86COFFArchiveReader,
                               createX86COFFObjectReader,
                               createX86COFFObjectWriter);
    **/
  }
  return new X86GNULDBackend(pConfig, new X86GNUInfo(pConfig.targets().triple()));
}

} // namespace of mcld

//===----------------------------------------------------------------------===//
// Force static initialization.
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeX86LDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheX86Target, createX86LDBackend);
}
