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
  : m_pRelocFactory(0),
    m_pGOT(0),
    m_pPLT(0),
    m_pRelDyn(0),
    m_pRelPLT(0),
    m_pDynamic(0) {
}

X86GNULDBackend::~X86GNULDBackend()
{
  if (0 != m_pRelocFactory)
    delete m_pRelocFactory;
  if (0 != m_pGOT)
    delete m_pGOT;
  if (0 != m_pPLT)
    delete m_pPLT;
  if(m_pRelDyn)
    delete m_pRelDyn;
  if(m_pRelPLT)
    delete m_pRelPLT;
  if (0 != m_pDynamic)
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
      createX86GOT(pLinker, pOutput.type());
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

void X86GNULDBackend::createX86GOT(MCLinker& pLinker, unsigned int pType)
{
  // get .got LDSection and create MCSectionData
  if( pType == Output::DynObj ) {
    LDSection& got = getDynObjFileFormat()->getGOT();
    m_pGOT = new X86GOT(got, pLinker.getOrCreateSectData(got));
  }
  else if( pType == Output::Exec) {
    LDSection& got = getExecFileFormat()->getGOT();
    m_pGOT = new X86GOT(got, pLinker.getOrCreateSectData(got));
  }
  else
    llvm::report_fatal_error(llvm::Twine("GOT is not support in ") +
                             ("output file type ") +
                             llvm::Twine(pType));

  // define symbol _GLOBAL_OFFSET_TABLE_ when .got create
  pLinker.defineSymbol<MCLinker::Force>("_GLOBAL_OFFSET_TABLE_",
                                        false,
                                        ResolveInfo::Object,
                                        ResolveInfo::Define,
                                        ResolveInfo::Local,
                                        0, // size
                                        0, // value
                                        pLinker.getLayout().getFragmentRef(*(m_pGOT->begin()), 0),
                                        ResolveInfo::Hidden);
}

void X86GNULDBackend::createX86PLTandRelPLT(MCLinker& pLinker,
                                            unsigned int pType)
{
  // Create .got section if it dosen't exist
  if(!m_pGOT)
    createX86GOT(pLinker, pType);

  // get .plt and .rel.plt LDSection
  if( pType == Output::DynObj ) {
    LDSection& plt = getDynObjFileFormat()->getPLT();
    LDSection& relplt = getDynObjFileFormat()->getRelPlt();
    // create MCSectionData and X86PLT
    m_pPLT = new X86PLT(plt, pLinker.getOrCreateSectData(plt), *m_pGOT,
			pType);
    // set info of .rel.plt to .plt
    relplt.setLinkInfo(&plt);
    // create MCSectionData and X86RelDynSection
    m_pRelPLT = new OutputRelocSection(relplt,
                                       pLinker.getOrCreateSectData(relplt),
                                       8);
  }
  else if( pType == Output::Exec ) {
    LDSection& plt = getExecFileFormat()->getPLT();
    LDSection& relplt = getExecFileFormat()->getRelPlt();
    // create MCSectionData and X86PLT
    m_pPLT = new X86PLT(plt, pLinker.getOrCreateSectData(plt), *m_pGOT,
			pType);
    // set info of .rel.plt to .plt
    relplt.setLinkInfo(&plt);
    // create MCSectionData and X86RelDynSection
    m_pRelPLT = new OutputRelocSection(relplt,
                                       pLinker.getOrCreateSectData(relplt),
                                       8);
  }
  else
    llvm::report_fatal_error(llvm::Twine("PLT is not support in ") +
                             ("output file type ") +
                             llvm::Twine(pType));
}

void X86GNULDBackend::createX86RelDyn(MCLinker& pLinker,
                                      unsigned int pType)
{
  // get .rel.dyn LDSection and create MCSectionData
  if( pType == Output::DynObj ) {
    LDSection& reldyn = getDynObjFileFormat()->getRelDyn();
    // create MCSectionData and X86RelDynSection
    m_pRelDyn = new OutputRelocSection(reldyn,
                                       pLinker.getOrCreateSectData(reldyn),
                                       8);
  }
  else if( pType == Output::Exec) {
    LDSection& reldyn = getExecFileFormat()->getRelDyn();
    // create MCSectionData and X86RelDynSection
    m_pRelDyn = new OutputRelocSection(reldyn,
                                       pLinker.getOrCreateSectData(reldyn),
                                       8);
  }
  else
    llvm::report_fatal_error(llvm::Twine("Dynamic Relocation ") +
                             ("is not support in output file type ") +
                             llvm::Twine(pType));
}

bool X86GNULDBackend::isSymbolNeedsPLT(const ResolveInfo& pSym,
                                       unsigned int pType,
                                       const MCLDInfo& pLDInfo)
{
  return((Output::DynObj == pType)
         &&(ResolveInfo::Function == pSym.type())
         &&(pSym.isDyn() || pSym.isUndef() ||
            isSymbolPreemptible(pSym, pType, pLDInfo))
        );
}

bool X86GNULDBackend::isSymbolNeedsDynRel(const ResolveInfo& pSym,
                                          unsigned int pType,
                                          bool isAbsReloc)
{
  if(pSym.isUndef() && (pType==Output::Exec))
    return false;
  if(pSym.isAbsolute())
    return false;
  if(pType==Output::DynObj && isAbsReloc)
    return true;
  if(pSym.isDyn() || pSym.isUndef())
    return true;

  return false;
}

bool X86GNULDBackend::isSymbolPreemptible(const ResolveInfo& pSym,
                                         unsigned int pType,
                                         const MCLDInfo& pLDInfo) const
{
  if(pSym.other() != ResolveInfo::Default)
    return false;

  if(pType != Output::DynObj)
    return false;

  if(pLDInfo.options().Bsymbolic())
    return false;

  return true;
}

void X86GNULDBackend::scanLocalReloc(Relocation& pReloc,
                                     MCLinker& pLinker,
                                     const MCLDInfo& pLDInfo,
                                     unsigned int pType)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

  switch(pReloc.type()){

    case ELF::R_386_32:
      // If buiding PIC object (shared library or PIC executable),
      // a dynamic relocations with RELATIVE type to this location is needed.
      // Reserve an entry in .rel.dyn
      if(Output::DynObj == pType) {
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createX86RelDyn(pLinker, pType);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        // set Rel bit
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      return;

    case ELF::R_386_GOTOFF:
    case ELF::R_386_GOTPC:
      // A GOT section is needed
      if(!m_pGOT)
        createX86GOT(pLinker, pType);
      return;

    default:
      llvm::report_fatal_error(llvm::Twine("unexpected reloc ") +
                               llvm::Twine((int) pReloc.type()) +
                               llvm::Twine("in object file"));
      break;
  } // end switch
}

void X86GNULDBackend::scanGlobalReloc(Relocation& pReloc,
                                      MCLinker& pLinker,
                                      const MCLDInfo& pLDInfo,
                                      unsigned int pType)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

  switch(pReloc.type()) {
    case ELF::R_386_32:
      // Absolute relocation type, symbol may needs PLT entry or
      // dynamic relocation entry
      if(isSymbolNeedsPLT(*rsym, pType, pLDInfo)) {
        // create plt for this symbol if it does not have one
        if(!(rsym->reserved() & ReservePLT)){
          // create .plt and .rel.plt if not exist
          if(!m_pPLT)
            createX86PLTandRelPLT(pLinker, pType);
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

      if(isSymbolNeedsDynRel(*rsym, pType, true)) {
        // symbol needs dynamic relocation entry, reserve an entry in .rel.dyn
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createX86RelDyn(pLinker, pType);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        // set Rel bit
        rsym->setReserved(rsym->reserved() | ReserveRel);
      }
      return;

    case ELF::R_386_GOTOFF:
    case ELF::R_386_GOTPC: {
      // A GOT section is needed
      if(!m_pGOT)
        createX86GOT(pLinker, pType);
      return;
    }

    case ELF::R_386_PLT32:
      // A PLT entry is needed when building shared library

      // return if we already create plt for this symbol
      if(rsym->reserved() & ReservePLT)
        return;

      // if symbol is defined in the ouput file and it's not
      // preemptible, no need plt
      if(rsym->isDefine() && !rsym->isDyn() &&
         !isSymbolPreemptible(*rsym, pType, pLDInfo)) {
        return;
      }

      // create .plt and .rel.plt if not exist
      if(!m_pPLT)
         createX86PLTandRelPLT(pLinker, pType);
      // Symbol needs PLT entry, we need to reserve a PLT entry
      // and the corresponding GOT and dynamic relocation entry
      // in .got and .rel.plt. (GOT entry will be reserved simultaneously
      // when calling X86PLT->reserveEntry())
      m_pPLT->reserveEntry();
      m_pRelPLT->reserveEntry(*m_pRelocFactory);
      // set PLT bit
      rsym->setReserved(rsym->reserved() | ReservePLT);
      return;

    case ELF::R_386_GOT32:
      // Symbol needs GOT entry, reserve entry in .got
      // return if we already create GOT for this symbol
      if(rsym->reserved() & (ReserveGOT | GOTRel))
        return;
      if(!m_pGOT)
        createX86GOT(pLinker, pType);
      m_pGOT->reserveEntry();
      // If building shared object or the symbol is undefined, a dynamic
      // relocation is needed to relocate this GOT entry. Reserve an
      // entry in .rel.dyn
      if(Output::DynObj == pType || rsym->isUndef() || rsym->isDyn()) {
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createX86RelDyn(pLinker, pType);
        m_pRelDyn->reserveEntry(*m_pRelocFactory);
        // set GOTRel bit
        rsym->setReserved(rsym->reserved() | GOTRel);
        return;
      }
      // set GOT bit
      rsym->setReserved(rsym->reserved() | ReserveGOT);
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
                                     MCLinker& pLinker,
                                     const MCLDInfo& pLDInfo,
                                     unsigned int pType)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  assert(0 != rsym && "ResolveInfo of relocation not set while scanRelocation");

  // Scan relocation type to determine if an GOT/PLT/Dynamic Relocation
  // entries should be created.
  // FIXME: Below judgements concern only .so is generated as output
  // FIXME: Below judgements concren nothing about TLS related relocation

  // A refernece to symbol _GLOBAL_OFFSET_TABLE_ implies that a .got section
  // is needed
  if((NULL == m_pGOT) && (0 == strcmp(rsym->name(), "_GLOBAL_OFFSET_TABLE_"))) {
    createX86GOT(pLinker, pType);
  }

  // rsym is local
  if(rsym->isLocal())
    scanLocalReloc(pReloc, pLinker, pLDInfo, pType);

  // rsym is global
  else if(rsym->isGlobal())
    scanGlobalReloc(pReloc, pLinker, pLDInfo, pType);

}

uint64_t X86GNULDBackend::emitSectionData(const Output& pOutput,
                                          const LDSection& pSection,
                                          const MCLDInfo& pInfo,
                                          MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  ELFDynObjFileFormat* FileFormat = getDynObjFileFormat();
  assert(FileFormat &&
         "DynObjFileFormat is NULL in X86GNULDBackend::emitSectionData!");

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

  pRegion.sync();

  return RegionSize;
}
uint32_t X86GNULDBackend::machine() const
{
  return ELF::EM_386;
}

X86GOT& X86GNULDBackend::getGOT()
{
  assert(0 != m_pGOT);
  return *m_pGOT;
}

const X86GOT& X86GNULDBackend::getGOT() const
{
  assert(0 != m_pGOT);
  return *m_pGOT;
}

X86PLT& X86GNULDBackend::getPLT()
{
  assert(0 != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

const X86PLT& X86GNULDBackend::getPLT() const
{
  assert(0 != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

OutputRelocSection& X86GNULDBackend::getRelDyn()
{
  assert(0 != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

const OutputRelocSection& X86GNULDBackend::getRelDyn() const
{
  assert(0 != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

OutputRelocSection& X86GNULDBackend::getRelPLT()
{
  assert(0 != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

const OutputRelocSection& X86GNULDBackend::getRelPLT() const
{
  assert(0 != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

unsigned int
X86GNULDBackend::getTargetSectionOrder(const Output& pOutput,
                                       const LDSection& pSectHdr) const
{
  ELFFileFormat* file_format = NULL;
  switch (pOutput.type()) {
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    case Output::Object:
    default:
      assert(0 && "Not support yet.\n");
      break;
  }

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

/// finalizeSymbol - finalize the symbol value
/// If the symbol's reserved field is not zero, MCLinker will call back this
/// function to ask the final value of the symbol
bool X86GNULDBackend::finalizeSymbol(LDSymbol& pSymbol) const
{
  return false;
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
