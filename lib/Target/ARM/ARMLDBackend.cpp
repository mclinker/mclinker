//===- ARMLDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/ADT/Triple.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/ErrorHandling.h>

#include <mcld/LD/SectionMap.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/TargetRegistry.h>

#include "ARM.h"
#include "ARMLDBackend.h"
#include "ARMRelocationFactory.h"

using namespace mcld;

namespace {

void
emitDTNULL(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(0);
}

void
emitDTNeeded(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  //FIXME: d_un.d_val needs an index into string table.
  pEntry->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(0);
}

void
emitDTPLTRelSZ(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getRelPlt().size());
}

void
emitDTPLTGOT(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getGOT().addr());
}

void emitDTHash(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getHashTab().addr());
}

void
emitDTStrTab(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getStrTab().addr());
}

void
emitDTSymTab(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getSymTab().addr());
}

void
emitDTRela(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat) {
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getRelaDyn().addr());
}

void
emitDTRelaSZ(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getRelaDyn().size());
}

void
emitDTStrSZ(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getStrTab().size());
}

void
emitDTSymEnt(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_val =
    static_cast<llvm::ELF::Elf32_Word>(sizeof(llvm::ELF::Elf32_Sym));
}

void
emitDTInit(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getInit().addr());
}

void
emitDTFini(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getFini().addr());
}

void
emitDTSOName(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  //FIXME: d_un.d_val needs an index into string table.
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Word>(0);
}

void
emitDTSymbolic(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(0);
}

void
emitDTRel(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getRelDyn().addr());
}

void
emitDTRelSZ(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getRelDyn().size());
}

void
emitDTRelEnt(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_val =
    static_cast<llvm::ELF::Elf32_Word>(sizeof(llvm::ELF::Elf32_Rel));
}

void
emitDTPLTRel(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
   pEntry->d_un.d_ptr =
     static_cast<llvm::ELF::Elf32_Addr>(llvm::ELF::DT_REL);
}

void
emitDTJMPRel(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getRelPlt().addr());
}

void
emitDTInitArray(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getInitArray().addr());
}

void
emitDTFiniArray(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getFiniArray().addr());
}

void
emitDTInitArraySZ(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getInitArray().size());
}

void
emitDTFiniArraySZ(llvm::ELF::Elf32_Dyn* pEntry, ELFDynObjFileFormat* FileFormat)
{
  pEntry->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getFiniArray().size());
}

const unsigned int dynamic_type_num = 33;
typedef void (*DynamicEntryFn)(llvm::ELF::Elf32_Dyn*, ELFDynObjFileFormat*);
DynamicEntryFn emitDynamicEntry[] = {
  emitDTNULL,
  emitDTNeeded,
  emitDTPLTRelSZ,
  emitDTPLTGOT,
  emitDTHash,
  emitDTStrTab,
  emitDTSymTab,
  emitDTRela,
  emitDTRelaSZ,
  0, // DT_RELAENT
  emitDTStrSZ,
  emitDTSymEnt,
  emitDTInit,
  emitDTFini,
  emitDTSOName,
  0, //DT_RPATH
  emitDTSymbolic,
  emitDTRel,
  emitDTRelSZ,
  emitDTRelEnt,
  emitDTPLTRel,
  0, // DT_DEBUG
  0, // DT_TEXTREL
  emitDTJMPRel,
  0, // DT_BIND_NOW
  emitDTInitArray,
  emitDTFiniArray,
  emitDTInitArraySZ,
  emitDTFiniArraySZ,
  0, // DT_RUNPATH
  0, // DT_FLAGS
  0, // DT_ENCODING
  0, // DT_PREINIT_ARRAY
  0 // DT_PREINIT_ARRAYSZ
};

} // end namespace

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

  for (ELF32DynList::const_iterator it = m_ELF32DynList.begin(),
       ie = m_ELF32DynList.end(); it != ie; ++it)
    free(*it);

}

bool ARMGNULDBackend::initRelocFactory(const MCLinker& pLinker)
{
  if (NULL == m_pRelocFactory) {
    m_pRelocFactory = new ARMRelocationFactory(1024, *this);
    m_pRelocFactory->setLayout(pLinker.getLayout());
  }
  return true;
}

RelocationFactory* ARMGNULDBackend::getRelocFactory()
{
  assert(m_pRelocFactory!= NULL);
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
  m_pEXIDX        = &pLinker.getOrCreateOutputSectHdr(".ARM.exidx",
                                                      LDFileFormat::Target,
                                                      ELF::SHT_ARM_EXIDX,
                                                      ELF::SHF_ALLOC | ELF::SHF_LINK_ORDER);
  m_pEXTAB        = &pLinker.getOrCreateOutputSectHdr(".ARM.extab",
                                                      LDFileFormat::Target,
                                                      ELF::SHT_PROGBITS,
                                                      ELF::SHF_ALLOC);
  m_pAttributes   = &pLinker.getOrCreateOutputSectHdr(".ARM.attributes",
                                                      LDFileFormat::Target,
                                                      ELF::SHT_ARM_ATTRIBUTES,
                                                      0);
}

void ARMGNULDBackend::initTargetSymbols(MCLinker& pLinker)
{
  // create symbol _GLOBAL_OFFSET_TABLE_ if .got section exist
  if(m_pGOT) {
    pLinker.defineSymbol(llvm::StringRef("_GLOBAL_OFFSET_TABLE_"),
                         false,
                         ResolveInfo::Object,
                         ResolveInfo::Define,
                         ResolveInfo::Local,
                         0,
                         pLinker.getLayout().getFragmentRef(*(m_pGOT->begin()), 0),
                         ResolveInfo::Hidden);
  }
}

void ARMGNULDBackend::preLayout(const Output& pOutput,
                                const MCLDInfo& pInfo,
                                MCLinker& pLinker)
{
  // when building shared object, the .got and .synamic section are needed
  if(pOutput.type() == Output::DynObj) {
    if(!m_pGOT)
      createARMGOT(pLinker);
  //TODO: Add _GLOBAL_OFFSET_TABLE_ simultaneously when .got created
  }

  sizeELF32Dynamic(pInfo);
}

void ARMGNULDBackend::postLayout(const Output& pOutput,
                                 const MCLDInfo& pInfo,
                                 MCLinker& pLinker)
{
  applyELF32Dynamic();
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

void ARMGNULDBackend::createARMPLTandRelPLT(MCLinker& pLinker)
{
  // Create .got section if it dosen't exist
  if(!m_pGOT)
    createARMGOT(pLinker);

  // Create .plt section
  LDSection& plt  = pLinker.getOrCreateOutputSectHdr(".plt",
                                                     LDFileFormat::Target,
                                                     ELF::SHT_PROGBITS,
                                                     ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);
  // create MCSectionData and ARMPLT
  m_pPLT = new ARMPLT(plt, pLinker.getOrCreateSectData(plt), *m_pGOT);

  // create .rel.plt section
  LDSection& relplt = pLinker.getOrCreateOutputSectHdr(".rel.plt",
                                                       LDFileFormat::Relocation,
                                                       ELF::SHT_REL,
                                                       ELF::SHF_ALLOC);
  // set info of .rel.plt to .plt
  relplt.setLinkInfo(&plt);
  // create MCSectionData and ARMRelDynSection
  m_pRelPLT = new ARMDynRelSection(relplt, pLinker.getOrCreateSectData(relplt), 8);
}

void ARMGNULDBackend::createARMRelDyn(MCLinker& pLinker)
{
  LDSection& reldyn = pLinker.getOrCreateOutputSectHdr(".rel.dyn",
                                                       LDFileFormat::Relocation,
                                                       ELF::SHT_REL,
                                                       ELF::SHF_ALLOC);
  // create MCSectionData and ARMRelDynSection
  m_pRelDyn = new ARMDynRelSection(reldyn, pLinker.getOrCreateSectData(reldyn), 8);
}

bool ARMGNULDBackend::isSymbolNeedsPLT(const ResolveInfo& pSym,
                                       unsigned int pType)
{
  return((Output::DynObj == pType)
         &&(ResolveInfo::Function == pSym.type())
         &&(pSym.isDyn() || pSym.isUndef())
        );
}

bool ARMGNULDBackend::isSymbolNeedsDynRel(const ResolveInfo& pSym,
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

void ARMGNULDBackend::createELF32DynamicEntry(llvm::ELF::Elf32_Sword pTag,
                                              size_t& pSectionSize)
{
  size_t EntrySize = sizeof(llvm::ELF::Elf32_Dyn);

  llvm::ELF::Elf32_Dyn* Entry = 0;
  Entry = static_cast<llvm::ELF::Elf32_Dyn*>(malloc(EntrySize));

  pSectionSize += EntrySize;

  Entry->d_tag = pTag;

  m_ELF32DynList.push_back(Entry);
}

void ARMGNULDBackend::scanRelocation(Relocation& pReloc,
                                     MCLinker& pLinker,
                                     unsigned int pType)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  assert(0 != rsym && "ResolveInfo of relocation not set while scanRelocation");

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
    // A refernece to symbol _GLOBAL_OFFSET_TABLE_ implies that a .got section
    // is needed
    if(0 == strcmp(rsym->name(), "_GLOBAL_OFFSET_TABLE_")) {
      if(!m_pGOT)
        createARMGOT(pLinker);
    }

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
          // break if we already create plt for this symbol
          if(rsym->reserved() & 0x8u)
            break;
          // create .plt and .rel.plt if not exist
          if(!m_pPLT)
            createARMPLTandRelPLT(pLinker);
          // Symbol needs PLT entry, we need to reserve a PLT entry
          // and the corresponding GOT and dynamic relocation entry
          // in .got and .rel.plt. (GOT entry will be reserved simultaneously
          // when calling ARMPLT->reserveEntry())
          m_pPLT->reserveEntry();
          m_pRelPLT->reserveEntry(*m_pRelocFactory);
          // set PLT bit
          rsym->setReserved(rsym->reserved() | 0x8u);
        }
        if(isSymbolNeedsDynRel(*rsym, pType, true)) {
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
        if(isSymbolNeedsDynRel(*rsym, pType, false)) {
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
        // create .plt and .rel.plt if not exist
        if(!m_pPLT)
           createARMPLTandRelPLT(pLinker);
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

void ARMGNULDBackend::sizeELF32Dynamic(const MCLDInfo& pLDInfo) {
  unsigned int type = pLDInfo.output().type();

  if (type == Output::Object) {
    llvm::report_fatal_error(
    "Relocatable object file should not have .dynamic section!");
  }

  assert(m_pDynObjFileFormat &&
         "m_pDynObjFileFormat in ARMGNULDBackend is NULL!");

  LDSection* dynamic = &(m_pDynObjFileFormat->getDynamic());
  assert(dynamic && ".dynamic section donesn't exist!");

  size_t SectionSize = 0;
  size_t EntrySize = sizeof(llvm::ELF::Elf32_Dyn);

  InputTree::const_bfs_iterator input = pLDInfo.inputs().bfs_begin();
  InputTree::const_bfs_iterator inputEnd = pLDInfo.inputs().bfs_end();

  while (input != inputEnd) {
    // --add-needed
    if ((*input)->attribute()->isAddNeeded()) {
      // --no-as-need
      if (!(*input)->attribute()->isAsNeeded())
        createELF32DynamicEntry(llvm::ELF::DT_NEEDED, SectionSize);

      // --as-needed
      else if ((*input)->isNeeded())
        createELF32DynamicEntry(llvm::ELF::DT_NEEDED, SectionSize);
    }

    ++input;
  }

  if (type == Output::DynObj) {
    createELF32DynamicEntry(llvm::ELF::DT_SONAME, SectionSize);

    if (pLDInfo.options().Bsymbolic()) {
      createELF32DynamicEntry(llvm::ELF::DT_SYMBOLIC, SectionSize);
    }
  }

  createELF32DynamicEntry(llvm::ELF::DT_INIT, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_FINI, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_INIT_ARRAY, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_FINI_ARRAY, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_INIT_ARRAYSZ, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_FINI_ARRAYSZ, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_HASH, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_STRTAB, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_SYMTAB, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_STRSZ, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_SYMENT, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_PLTGOT, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_PLTRELSZ, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_PLTREL, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_JMPREL, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_REL, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_RELSZ, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_RELENT, SectionSize);
  createELF32DynamicEntry(llvm::ELF::DT_NULL, SectionSize);

  dynamic->setSize(SectionSize);
}

void ARMGNULDBackend::applyELF32Dynamic() const
{
  DynamicEntryFn emitFn = 0;

  for (ELF32DynList::const_iterator it = m_ELF32DynList.begin(),
       ie = m_ELF32DynList.end(); it != ie; ++it) {
    llvm::ELF::Elf32_Sword tag = (*it)->d_tag;

    assert((0 <= tag && tag < dynamic_type_num) &&
            "Unknown dynamic section tags");

    // Standard dynamic section tags
    emitFn = emitDynamicEntry[tag];

    if (emitFn) {
      assert(m_pDynObjFileFormat &&
             "m_pDynObjFileFormat in ARMGNULDBackend is NULL!");

      emitFn((*it), m_pDynObjFileFormat);
    }

    else
      llvm::report_fatal_error("Unsupported dynamic section tags");
  }
}

void ARMGNULDBackend::emitELF32Dynamic(MemoryRegion& pRegion) const
{
  llvm::ELF::Elf32_Dyn* buffer =
    reinterpret_cast<llvm::ELF::Elf32_Dyn*>(pRegion.getBuffer());

  for (ELF32DynList::const_iterator it = m_ELF32DynList.begin(),
       ie = m_ELF32DynList.end(); it != ie; ++it, ++buffer) {
    memcpy(buffer, (*it), sizeof(llvm::ELF::Elf32_Dyn));
  }
}

uint64_t ARMGNULDBackend::emitSectionData(const Output& pOutput,
                                          const LDSection& pSection,
                                          const MCLDInfo& pInfo,
                                          MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  const char* SectionName = pSection.name().c_str();

  unsigned int EntrySize = 0;
  uint64_t RegionSize = 0;

  if (!std::strcmp(SectionName,".ARM.attributes")) {
    // FIXME: Unsupport emitting .ARM.attributes.
    return 0;
  }

  else if (!std::strcmp(SectionName, ".plt")) {
    assert(m_pPLT && "emitSectionData failed, m_pPLT is NULL!");

    unsigned char* buffer = pRegion.getBuffer();

    m_pPLT->applyPLT0();
    m_pPLT->applyPLT1();

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
      memcpy(buffer + RegionSize, plt1->getContent(), EntrySize);
      RegionSize += EntrySize;
      ++it;
    }
  }

  else if (!std::strcmp(SectionName,".got")) {
    assert(m_pGOT && "emitSectionData failed, m_pGOT is NULL!");

    uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

    GOTEntry* got = 0;
    EntrySize = m_pGOT->getEntrySize();

    for (ARMGOT::iterator it = m_pGOT->begin(),
         ie = m_pGOT->end(); it != ie; ++it, ++buffer) {
      got = &(llvm::cast<GOTEntry>((*it)));
      *buffer = static_cast<uint32_t>(got->getContent());
      RegionSize += EntrySize;
    }
  }

  else if (!std::strcmp(SectionName,".dynamic")) {
    emitELF32Dynamic(pRegion);
  }

  else
    llvm::report_fatal_error("unsupported section name "
                             + pSection.name() + " !");

  pRegion.sync();

  return RegionSize;
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

unsigned int
ARMGNULDBackend::getTargetSectionOrder(const LDSection& pSectHdr) const
{
  if (pSectHdr.name() == ".got" || pSectHdr.name() == ".got.plt")
    return SHO_DATA;

  if (pSectHdr.name() == ".plt")
    return SHO_PLT;

  return SHO_UNDEFINED;
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
