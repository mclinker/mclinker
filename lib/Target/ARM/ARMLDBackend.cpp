//===- ARMLDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARM.h"
#include "ARMLDBackend.h"
#include "ARMRelocationFactory.h"
#include <mcld/LD/SectionMap.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/TargetRegistry.h>
#include <llvm/ADT/Triple.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/ErrorHandling.h>

using namespace mcld;

ARMGNULDBackend::ARMGNULDBackend()
  : m_pRelocFactory(0),
    m_pGOT(0),
    m_pPLT(0),
    m_pRelDyn(0),
    m_pRelPLT(0) {
}

ARMGNULDBackend::~ARMGNULDBackend()
{
  if(m_pGOT)
    delete m_pGOT;
  if(m_pPLT)
    delete m_pPLT;
  if(m_pRelDyn)
    delete m_pRelDyn;
  if(m_pRelPLT)
    delete m_pRelPLT;
}

RelocationFactory* ARMGNULDBackend::getRelocFactory()
{
  if (NULL == m_pRelocFactory)
    m_pRelocFactory = new ARMRelocationFactory(1024, *this);
  return m_pRelocFactory;
}

uint32_t ARMGNULDBackend::machine() const
{
  return ELF::EM_ARM;
}

bool ARMGNULDBackend::isLittleEndian() const
{
  return true;
}

bool ARMGNULDBackend::initTargetSectionMap(SectionMap& pSectionMap)
{
  if (!pSectionMap.push_back(".ARM.exidx", ".ARM.exidx") ||
      !pSectionMap.push_back(".ARM.extab", ".ARM.extab") ||
      !pSectionMap.push_back(".ARM.attributes", ".ARM.attributes"))
    return false;
  return true;
}

void ARMGNULDBackend::initTargetSections(MCLinker& pLinker)
{
  m_pEXIDX        = &pLinker.createSectHdr(".ARM.exidx",
                                          LDFileFormat::Target,
                                          ELF::SHT_ARM_EXIDX,
                                                  ELF::SHF_ALLOC | ELF::SHF_LINK_ORDER);
  m_pEXTAB        = &pLinker.createSectHdr(".ARM.extab",
                                          LDFileFormat::Target,
                                          ELF::SHT_PROGBITS,
                                          ELF::SHF_ALLOC);
  m_pAttributes   = &pLinker.createSectHdr(".ARM.attributes",
                                          LDFileFormat::Target,
                                          ELF::SHT_ARM_ATTRIBUTES,
                                          ELF::SHF_ALLOC);
}

void ARMGNULDBackend::createARMGOT(MCLinker& pLinker)
{
  LDSection& got  = pLinker.getOrCreateOutputSectHdr(".got",
                                                     LDFileFormat::Target,
                                                     ELF::SHT_PROGBITS,
                                                     ELF::SHF_ALLOC | ELF::SHF_WRITE);

  // create MCSectionData and ARMGOT
  m_pGOT = new ARMGOT(got, pLinker.getOrCreateSectData(got));
}

void ARMGNULDBackend::createARMPLT(MCLinker& pLinker)
{
  // Create .got section if it dosen't exist
  if(!m_pGOT)
    createARMGOT(pLinker);
  LDSection& plt  = pLinker.getOrCreateOutputSectHdr(".plt",
                                                     LDFileFormat::Target,
                                                     ELF::SHT_PROGBITS,
                                                     ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  // create MCSectionData and ARMPLT
  m_pPLT = new ARMPLT(plt, pLinker.getOrCreateSectData(plt), *m_pGOT);
}

void ARMGNULDBackend::createARMRelDyn(MCLinker& pLinker)
{
  LDSection& reldyn = pLinker.getOrCreateOutputSectHdr(".rel.dyn",
                                                       LDFileFormat::Relocation,
                                                       ELF::SHT_REL,
                                                       ELF::SHF_ALLOC);
  // create MCSectionData and ARMRelDynSection
  unsigned int size = bitclass() / 8 * 2;
  m_pRelDyn = new ARMDynRelSection(reldyn, pLinker.getOrCreateSectData(reldyn), size);
}

void ARMGNULDBackend::createARMRelPLT(MCLinker& pLinker)
{
  LDSection& relplt = pLinker.getOrCreateOutputSectHdr(".rel.plt",
                                                       LDFileFormat::Relocation,
                                                       ELF::SHT_REL,
                                                       ELF::SHF_ALLOC);
  // create MCSectionData and ARMRelDynSection
  unsigned int size = bitclass() / 8 * 2;
  m_pRelPLT = new ARMDynRelSection(relplt, pLinker.getOrCreateSectData(relplt), size);
}

bool ARMGNULDBackend::isSymbolNeedsPLT(ResolveInfo& pSym,
                                       unsigned int pType)
{
  return((Output::DynObj == pType)
         &&(ResolveInfo::Function == pSym.type())
         &&(pSym.isDyn() || pSym.isUndef())
        );
}

bool ARMGNULDBackend::isSymbolNeedsDynRel(ResolveInfo& pSym,
                                          unsigned int pType)
{
  if(pSym.isUndef() && (pType==Output::Exec))
    return false;
  if(pSym.isAbsolute())
    return false;
  if(pType==Output::DynObj)
    return true;
  if(pSym.isDyn() || pSym.isUndef())
    return true;

  return false;
}

void ARMGNULDBackend::scanRelocation(Relocation& pReloc,
                                     MCLinker& pLinker,
                                     unsigned int pType)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

  // Scan relocation type to determine if an GOT/PLT/Dynamic Relocation
  // entries should be created.
  // FIXME: Below judgements concern only .so is generated as output
  // FIXME: Below judgements concren nothing about TLS related relocation

  // rsym is local symbol
  if(rsym->isLocal()) {
    switch(pReloc.type()){

      case ELF::R_ARM_ABS32:
      case ELF::R_ARM_ABS32_NOI: {
        // If buiding PIC object (shared library or PIC executable),
        // a dynamic relocations with RELATIVE type to this location is needed.
        // Reserve an entry in .rel.dyn
        if(Output::DynObj == pType) {
          // create .rel.dyn section if not exist
          if(!m_pRelDyn)
            createARMRelDyn(pLinker);
          m_pRelDyn->reserveEntry(*m_pRelocFactory);
          // set Rel bit
          rsym->setReserved(rsym->reserved() | 0x1u);
        }
        return;
      }

      case ELF::R_ARM_ABS16:
      case ELF::R_ARM_ABS12:
      case ELF::R_ARM_THM_ABS5:
      case ELF::R_ARM_ABS8:
      case ELF::R_ARM_BASE_ABS:
      case ELF::R_ARM_MOVW_ABS_NC:
      case ELF::R_ARM_MOVT_ABS:
      case ELF::R_ARM_THM_MOVW_ABS_NC:
      case ELF::R_ARM_THM_MOVT_ABS: {
        // If building PIC object (shared library or PIC executable),
        // a dynamic relocation for this location is needed.
        // Reserve an entry in .rel.dyn
        if(Output::DynObj == pType) {
          // create .rel.dyn section if not exist
          if(!m_pRelDyn)
            createARMRelDyn(pLinker);
          m_pRelDyn->reserveEntry(*m_pRelocFactory);
          // set Rel bit
          rsym->setReserved(rsym->reserved() | 0x1u);
        }
        return;
      }
      case ELF::R_ARM_GOTOFF32:
      case ELF::R_ARM_GOTOFF12: {
        // A GOT section is needed
        if(!m_pGOT)
          createARMGOT(pLinker);
        return;
      }

      case ELF::R_ARM_GOT_BREL:
      case ELF::R_ARM_GOT_PREL: {
        // A GOT entry is needed for these relocation type.
        // return if we already create GOT for this symbol
        if(rsym->reserved() & 0x6u)
          return;
        if(!m_pGOT)
          createARMGOT(pLinker);
        m_pGOT->reserveEntry();
        // If building shared object, a dynamic relocation with
        // type RELATIVE is needed to relocate this GOT entry.
        // Reserve an entry in .rel.dyn
        if(Output::DynObj == pType) {
          // create .rel.dyn section if not exist
          if(!m_pRelDyn)
            createARMRelDyn(pLinker);
          m_pRelDyn->reserveEntry(*m_pRelocFactory);
          // set GOTRel bit
          rsym->setReserved(rsym->reserved() | 0x4u);
          return;
        }
        // set GOT bit
        rsym->setReserved(rsym->reserved() | 0x2u);
        return;
      }

      case ELF::R_ARM_COPY:
      case ELF::R_ARM_GLOB_DAT:
      case ELF::R_ARM_JUMP_SLOT:
      case ELF::R_ARM_RELATIVE: {
        // These are relocation type for dynamic linker, shold not
        // appear in object file.
        llvm::report_fatal_error(llvm::Twine("unexpected reloc ") +
                                 llvm::Twine(pReloc.type()) +
                                 llvm::Twine("in object file"));
        break;
      }
      default: {
        break;
      }
    } // end switch
  } // end if(rsym->isLocal)

  // rsym is global symbol
  else if(rsym->isGlobal()) {
    switch(pReloc.type()) {

      case ELF::R_ARM_ABS32:
      case ELF::R_ARM_ABS16:
      case ELF::R_ARM_ABS12:
      case ELF::R_ARM_THM_ABS5:
      case ELF::R_ARM_ABS8:
      case ELF::R_ARM_BASE_ABS:
      case ELF::R_ARM_MOVW_ABS_NC:
      case ELF::R_ARM_MOVT_ABS:
      case ELF::R_ARM_THM_MOVW_ABS_NC:
      case ELF::R_ARM_THM_MOVT_ABS:
      case ELF::R_ARM_ABS32_NOI: {
        // Absolute relocation type, symbol may needs PLT entry or
        // dynamic relocation entry
        if(isSymbolNeedsPLT(*rsym, pType)) {
          // return if we already create plt for this symbol
          if(rsym->reserved() & 0x8u)
            return;
          // create .plt if not exist
          if(!m_pPLT)
            createARMPLT(pLinker);
          // create .rel.plt if not exist
          if(!m_pRelPLT)
            createARMRelPLT(pLinker);
          // Symbol needs PLT entry, we need to reserve a PLT entry
          // and the corresponding GOT and dynamic relocation entry
          // in .got and .rel.plt. (GOT entry will be reserved simultaneously
          // when calling ARMPLT->reserveEntry())
          m_pPLT->reserveEntry();
          m_pRelPLT->reserveEntry(*m_pRelocFactory);
          // set PLT bit
          rsym->setReserved(rsym->reserved() | 0x8u);
        }
        else if(isSymbolNeedsDynRel(*rsym, pType)) {
          // symbol needs dynamic relocation entry, reserve an entry in .rel.dyn
          // create .rel.dyn section if not exist
          if(!m_pRelDyn)
            createARMRelDyn(pLinker);
          m_pRelDyn->reserveEntry(*m_pRelocFactory);
          // set Rel bit
          rsym->setReserved(rsym->reserved() | 0x1u);
        }
        return;
      }

      case ELF::R_ARM_GOTOFF32:
      case ELF::R_ARM_GOTOFF12: {
        // A GOT section is needed
        if(!m_pGOT)
          createARMGOT(pLinker);
        return;
      }

      case ELF::R_ARM_REL32:
      case ELF::R_ARM_LDR_PC_G0:
      case ELF::R_ARM_SBREL32:
      case ELF::R_ARM_THM_PC8:
      case ELF::R_ARM_BASE_PREL:
      case ELF::R_ARM_MOVW_PREL_NC:
      case ELF::R_ARM_MOVT_PREL:
      case ELF::R_ARM_THM_MOVW_PREL_NC:
      case ELF::R_ARM_THM_MOVT_PREL:
      case ELF::R_ARM_THM_ALU_PREL_11_0:
      case ELF::R_ARM_THM_PC12:
      case ELF::R_ARM_REL32_NOI:
      case ELF::R_ARM_ALU_PC_G0_NC:
      case ELF::R_ARM_ALU_PC_G0:
      case ELF::R_ARM_ALU_PC_G1_NC:
      case ELF::R_ARM_ALU_PC_G1:
      case ELF::R_ARM_ALU_PC_G2:
      case ELF::R_ARM_LDR_PC_G1:
      case ELF::R_ARM_LDR_PC_G2:
      case ELF::R_ARM_LDRS_PC_G0:
      case ELF::R_ARM_LDRS_PC_G1:
      case ELF::R_ARM_LDRS_PC_G2:
      case ELF::R_ARM_LDC_PC_G0:
      case ELF::R_ARM_LDC_PC_G1:
      case ELF::R_ARM_LDC_PC_G2:
      case ELF::R_ARM_ALU_SB_G0_NC:
      case ELF::R_ARM_ALU_SB_G0:
      case ELF::R_ARM_ALU_SB_G1_NC:
      case ELF::R_ARM_ALU_SB_G1:
      case ELF::R_ARM_ALU_SB_G2:
      case ELF::R_ARM_LDR_SB_G0:
      case ELF::R_ARM_LDR_SB_G1:
      case ELF::R_ARM_LDR_SB_G2:
      case ELF::R_ARM_LDRS_SB_G0:
      case ELF::R_ARM_LDRS_SB_G1:
      case ELF::R_ARM_LDRS_SB_G2:
      case ELF::R_ARM_LDC_SB_G0:
      case ELF::R_ARM_LDC_SB_G1:
      case ELF::R_ARM_LDC_SB_G2:
      case ELF::R_ARM_MOVW_BREL_NC:
      case ELF::R_ARM_MOVT_BREL:
      case ELF::R_ARM_MOVW_BREL:
      case ELF::R_ARM_THM_MOVW_BREL_NC:
      case ELF::R_ARM_THM_MOVT_BREL:
      case ELF::R_ARM_THM_MOVW_BREL: {
        // Relative addressing relocation, may needs dynamic relocation
        if(isSymbolNeedsDynRel(*rsym, pType)) {
          // create .rel.dyn section if not exist
          if(!m_pRelDyn)
            createARMRelDyn(pLinker);
          m_pRelDyn->reserveEntry(*m_pRelocFactory);
          // set Rel bit
          rsym->setReserved(rsym->reserved() | 0x1u);
        }
        return;
      }

      case ELF::R_ARM_THM_CALL:
      case ELF::R_ARM_PLT32:
      case ELF::R_ARM_CALL:
      case ELF::R_ARM_JUMP24:
      case ELF::R_ARM_THM_JUMP24:
      case ELF::R_ARM_SBREL31:
      case ELF::R_ARM_PREL31:
      case ELF::R_ARM_THM_JUMP19:
      case ELF::R_ARM_THM_JUMP6:
      case ELF::R_ARM_THM_JUMP11:
      case ELF::R_ARM_THM_JUMP8: {
        // These are branch relocation (except PREL31)
        // A PLT entry is needed when building shared library

        // return if we already create plt for this symbol
        if(rsym->reserved() & 0x8u)
          return;
        // if symbol is defined in the ouput file and it's protected
        // or hidden, no need plt
        if(rsym->isDefine() && !rsym->isDyn()
           && (rsym->other() == ResolveInfo::Hidden
               || rsym->other() == ResolveInfo::Protected))
          return;
        // create .plt if not exist
        if(!m_pPLT)
           createARMPLT(pLinker);
        // create .rel.plt if not exist
        if(!m_pRelPLT)
           createARMRelPLT(pLinker);
        // Symbol needs PLT entry, we need to reserve a PLT entry
        // and the corresponding GOT and dynamic relocation entry
        // in .got and .rel.plt. (GOT entry will be reserved simultaneously
        // when calling ARMPLT->reserveEntry())
        m_pPLT->reserveEntry();
        m_pRelPLT->reserveEntry(*m_pRelocFactory);
        // set PLT bit
        rsym->setReserved(rsym->reserved() | 0x8u);
        return;
      }

      case ELF::R_ARM_GOT_BREL:
      case ELF::R_ARM_GOT_ABS:
      case ELF::R_ARM_GOT_PREL: {
        // Symbol needs GOT entry, reserve entry in .got
        // return if we already create GOT for this symbol
        if(rsym->reserved() & 0x6u)
          return;
        if(!m_pGOT)
          createARMGOT(pLinker);
        m_pGOT->reserveEntry();
        // If building shared object or the symbol is undefined, a dynamic
        // relocation is needed to relocate this GOT entry. Reserve an
        // entry in .rel.dyn
        if(Output::DynObj == pType || rsym->isUndef() || rsym->isDyn()) {
          // create .rel.dyn section if not exist
          if(!m_pRelDyn)
            createARMRelDyn(pLinker);
          m_pRelDyn->reserveEntry(*m_pRelocFactory);
          // set GOTRel bit
          rsym->setReserved(rsym->reserved() | 0x4u);
          return;
        }
        // set GOT bit
        rsym->setReserved(rsym->reserved() | 0x2u);
        return;
      }

      case ELF::R_ARM_COPY:
      case ELF::R_ARM_GLOB_DAT:
      case ELF::R_ARM_JUMP_SLOT:
      case ELF::R_ARM_RELATIVE: {
        // These are relocation type for dynamic linker, shold not
        // appear in object file.
        llvm::report_fatal_error(llvm::Twine("Unexpected reloc ") +
                                 llvm::Twine(pReloc.type()) +
                                 llvm::Twine("in object file"));
        break;
      }
      default: {
        break;
      }
    } // end switch
  } // end if(rsym->isGlobal)
}

uint64_t ARMGNULDBackend::emitSectionData(const LDSection& pSection,
                                          MemoryRegion& pRegion) const {
  return 0;
// TODO: Uncomment the following code when
//       the size of MemoryRegion is not zero.
/*  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  const char* SectionName = pSection.name().c_str();
  unsigned char* buffer = pRegion.getBuffer();

  unsigned int EntrySize = 0;
  uint64_t RegionSize = 0;

  if (!std::strcmp(SectionName,".got")) {
    GOTEntry* got = 0;
    EntrySize = m_pGOT->getEntryBytes();

    for (ARMGOT::iterator it = m_pGOT->begin(),
         ie = m_pGOT->end(); it != ie; ++it) {
      got = &(llvm::cast<GOTEntry>((*it)));
      memcpy(buffer + got->offset() ,&(got->getContent()), EntrySize);
      RegionSize += EntrySize;
    }
  }

  else if (!std::strcmp(SectionName, ".plt")) {
    ARMPLT::iterator it = m_pPLT->begin();
    unsigned int plt0_size = llvm::cast<ARMPLT0>((*it)).getEntrySize();

    memcpy(buffer, llvm::cast<ARMPLT0>((*it)).getContent(), plt0_size);
    RegionSize += plt0_size;
    ++it;

    ARMPLT1* plt1 = 0;
    ARMPLT::iterator ie = m_pPLT->end();
    while (it != ie) {
      plt1 = &(llvm::cast<ARMPLT1>(*it));
      EntrySize = plt1->getEntrySize();
      memcpy(buffer + plt1->offset(), plt1->getContent(), EntrySize);
      RegionSize += EntrySize;
    }
  }

  else
    llvm::report_fatal_error("unsupported section name!");

  return RegionSize;
*/
}

ARMGOT& ARMGNULDBackend::getGOT()
{
  assert(0 != m_pGOT && "GOT section not exist");
  return *m_pGOT;
}

const ARMGOT& ARMGNULDBackend::getGOT() const
{
  assert(0 != m_pGOT && "GOT section not exist");
  return *m_pGOT;
}

ARMPLT& ARMGNULDBackend::getPLT()
{
  assert(0 != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

const ARMPLT& ARMGNULDBackend::getPLT() const
{
  assert(0 != m_pPLT && "PLT section not exist");
  return *m_pPLT;
}

ARMDynRelSection& ARMGNULDBackend::getRelDyn()
{
  assert(0 != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}
   
const ARMDynRelSection& ARMGNULDBackend::getRelDyn() const
{
  assert(0 != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

ARMDynRelSection& ARMGNULDBackend::getRelPLT()
{
  assert(0 != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}
   
const ARMDynRelSection& ARMGNULDBackend::getRelPLT() const
{
  assert(0 != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

unsigned int ARMGNULDBackend::computeTargetSectionOrder(
  const LDSection& pSectHdr) const
{
  // TODO
  return ~(0U);
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createARMLDBackend - the help funtion to create corresponding ARMLDBackend
///
TargetLDBackend* createARMLDBackend(const llvm::Target& pTarget,
                                    const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
    /**
    return new ARMMachOLDBackend(createARMMachOArchiveReader,
                               createARMMachOObjectReader,
                               createARMMachOObjectWriter);
    **/
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
    /**
    return new ARMCOFFLDBackend(createARMCOFFArchiveReader,
                               createARMCOFFObjectReader,
                               createARMCOFFObjectWriter);
    **/
  }
  return new ARMGNULDBackend();
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeARMLDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheARMTarget, createARMLDBackend);
}
