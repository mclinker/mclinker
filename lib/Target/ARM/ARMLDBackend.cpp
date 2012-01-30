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
#include <mcld/MC/MCRegionFragment.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/TargetRegistry.h>

#include <cstring>

#include "ARM.h"
#include "ARMELFDynamic.h"
#include "ARMLDBackend.h"
#include "ARMRelocationFactory.h"

using namespace mcld;

ARMGNULDBackend::ARMGNULDBackend()
  : m_pRelocFactory(0),
    m_pGOT(0),
    m_pPLT(0),
    m_pRelDyn(0),
    m_pRelPLT(0),
    m_pDynamic(0),
    m_pEXIDX(0),
    m_pEXTAB(0),
    m_pAttributes(0) {
}

ARMGNULDBackend::~ARMGNULDBackend()
{
  if (m_pRelocFactory)
    delete m_pRelocFactory;
  if(m_pGOT)
    delete m_pGOT;
  if(m_pPLT)
    delete m_pPLT;
  if(m_pRelDyn)
    delete m_pRelDyn;
  if(m_pRelPLT)
    delete m_pRelPLT;
  if(m_pDynamic)
    delete m_pDynamic;
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
}

void ARMGNULDBackend::doPreLayout(const Output& pOutput,
                                  const MCLDInfo& pInfo,
                                  MCLinker& pLinker)
{
  // when building shared object, the .got section is needed
  if(pOutput.type() == Output::DynObj && (NULL == m_pGOT)) {
      createARMGOT(pLinker, pOutput);
  }
}

void ARMGNULDBackend::doPostLayout(const Output& pOutput,
                                   const MCLDInfo& pInfo,
                                   MCLinker& pLinker)
{
  // emit program headers
  if(pOutput.type() == Output::DynObj || pOutput.type() == Output::Exec)
    emitProgramHdrs(pLinker.getLDInfo().output());

  ELFFileFormat *file_format = NULL;
  switch (pOutput.type()) {
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    case Output::Object:
      break;
    default:
      llvm::report_fatal_error(llvm::Twine("Unsupported output file format: ") +
                               llvm::Twine(pOutput.type()));
      return;
  } // end of switch

  // apply PLT
  if (file_format->hasPLT()) {
    // Since we already have the size of LDSection PLT, m_pPLT should not be
    // NULL.
    assert(NULL != m_pPLT);
    m_pPLT->applyPLT0();
    m_pPLT->applyPLT1();
  }

  // apply GOT
  if (file_format->hasGOT()) {
    // Since we already have the size of GOT, m_pGOT should not be NULL.
    assert(NULL != m_pGOT);
    if (pOutput.type() == Output::DynObj)
      m_pGOT->applyGOT0(file_format->getDynamic().addr());
    else {
      // executable file and object file? should fill with zero.
      m_pGOT->applyGOT0(0);
    }
  }
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
ARMELFDynamic& ARMGNULDBackend::dynamic()
{
  if (NULL == m_pDynamic)
    m_pDynamic = new ARMELFDynamic(*this);

  return *m_pDynamic;
}

/// dynamic - the dynamic section of the target machine.
/// Use co-variant return type to return its own dynamic section.
const ARMELFDynamic& ARMGNULDBackend::dynamic() const
{
  assert( NULL != m_pDynamic);
  return *m_pDynamic;
}

bool ARMGNULDBackend::isPIC(const Output& pOutput, const MCLDInfo& pLDInfo) const
{
  return (pOutput.type() == Output::DynObj);
}

void ARMGNULDBackend::createARMGOT(MCLinker& pLinker, const Output& pOutput)
{
  // get .got LDSection and create MCSectionData
  ELFFileFormat* file_format = NULL;
  switch (pOutput.type()) {
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    default: 
      llvm::report_fatal_error(llvm::Twine("GOT is not support in ") +
                               "output file type " +
                               llvm::Twine(pOutput.type()));
      return;
  } // end of switch

  LDSection& got = file_format->getGOT();
  m_pGOT = new ARMGOT(got, pLinker.getOrCreateSectData(got));

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

void ARMGNULDBackend::createARMPLTandRelPLT(MCLinker& pLinker,
                                            const Output& pOutput)
{
  // Create .got section if it doesn't exist
  if(!m_pGOT)
    createARMGOT(pLinker, pOutput);

  ELFFileFormat* file_format = NULL;
  switch (pOutput.type()) {
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    default: 
      llvm::report_fatal_error(llvm::Twine("PLT is not support in ") +
                               "output file type " +
                               llvm::Twine(pOutput.type()));
      return;
  } // end of switch

  // get .plt and .rel.plt LDSection
  LDSection& plt = file_format->getPLT();
  LDSection& relplt = file_format->getRelPlt();
  // create MCSectionData and ARMPLT
  m_pPLT = new ARMPLT(plt, pLinker.getOrCreateSectData(plt), *m_pGOT);
  // set info of .rel.plt to .plt
  relplt.setLinkInfo(&plt);
  // create MCSectionData and ARMRelDynSection
  m_pRelPLT = new OutputRelocSection(relplt,
                                     pLinker.getOrCreateSectData(relplt),
                                     8);
}

void ARMGNULDBackend::createARMRelDyn(MCLinker& pLinker,
                                      const Output& pOutput)
{
  ELFFileFormat* file_format = NULL;
  switch (pOutput.type()) {
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    default: 
    llvm::report_fatal_error(llvm::Twine("Dynamic Relocation ") +
                             ("is not support in output file type ") +
                             llvm::Twine(pOutput.type()));
      return;
  } // end of switch

  // get .rel.dyn LDSection and create MCSectionData
  LDSection& reldyn = file_format->getRelDyn();
  // create MCSectionData and ARMRelDynSection
  m_pRelDyn = new OutputRelocSection(reldyn,
                                     pLinker.getOrCreateSectData(reldyn),
                                     8);
}

bool ARMGNULDBackend::isSymbolNeedsPLT(const ResolveInfo& pSym,
                                       const Output& pOutput,
                                       const MCLDInfo& pLDInfo) const
{
  return (Output::DynObj == pOutput.type() &&
          ResolveInfo::Function == pSym.type() &&
          (pSym.isDyn() || pSym.isUndef() ||
            isSymbolPreemptible(pSym, pOutput, pLDInfo)));
}

bool ARMGNULDBackend::isSymbolNeedsDynRel(const ResolveInfo& pSym,
                                          const Output& pOutput,
                                          bool isAbsReloc) const
{
  if(pSym.isUndef() && (Output::Exec == pOutput.type()))
    return false;
  if(pSym.isAbsolute())
    return false;
  if(Output::DynObj == pOutput.type() && isAbsReloc)
    return true;
  if(pSym.isDyn() || pSym.isUndef())
    return true;

  return false;
}

bool ARMGNULDBackend::isSymbolPreemptible(const ResolveInfo& pSym,
                                          const Output& pOutput,
                                          const MCLDInfo& pLDInfo) const
{
  if(pSym.other() != ResolveInfo::Default)
    return false;

  if(Output::DynObj != pOutput.type())
    return false;

  if(pLDInfo.options().Bsymbolic())
    return false;

  return true;
}

/// checkValidReloc - When we attempt to generate a dynamic relocation for
/// ouput file, check if the relocation is supported by dynamic linker.
void ARMGNULDBackend::checkValidReloc(Relocation& pReloc,
                                      const MCLDInfo& pLDInfo,
                                      const Output& pOutput) const
{
  // If not building shared object, no relocation type is invalid
  // FIXME: This should be check not building PIC
  if (isPIC(pOutput, pLDInfo))
    return;

  switch(pReloc.type()) {
    case ELF::R_ARM_RELATIVE:
    case ELF::R_ARM_COPY:
    case ELF::R_ARM_GLOB_DAT:
    case ELF::R_ARM_JUMP_SLOT:
    case ELF::R_ARM_ABS32:
    case ELF::R_ARM_ABS32_NOI:
    case ELF::R_ARM_PC24:
    case ELF::R_ARM_TLS_DTPMOD32:
    case ELF::R_ARM_TLS_DTPOFF32:
    case ELF::R_ARM_TLS_TPOFF32:
      break;

    default:
      llvm::report_fatal_error(llvm::Twine("Attempt to generate unsupported") +
                               llvm::Twine("relocation type ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine(" for symbol ") +
                               llvm::Twine(pReloc.symInfo()->name()) +
                               llvm::Twine(", recompile with -fPIC")
                              );
      break;
  }
}

void ARMGNULDBackend::scanLocalReloc(Relocation& pReloc,
                                     MCLinker& pLinker,
                                     const MCLDInfo& pLDInfo,
                                     const Output& pOutput)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

  switch(pReloc.type()){

    case ELF::R_ARM_ABS32:
    case ELF::R_ARM_ABS32_NOI: {
      // If buiding PIC object (shared library or PIC executable),
      // a dynamic relocations with RELATIVE type to this location is needed.
      // Reserve an entry in .rel.dyn
      if(Output::DynObj == pOutput.type()) {
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createARMRelDyn(pLinker, pOutput);
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
      if(Output::DynObj == pOutput.type()) {
        checkValidReloc(pReloc, pLDInfo, pOutput);
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createARMRelDyn(pLinker, pOutput);
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
        createARMGOT(pLinker, pOutput);
      return;
    }

    case ELF::R_ARM_GOT_BREL:
    case ELF::R_ARM_GOT_PREL: {
      // A GOT entry is needed for these relocation type.
      // return if we already create GOT for this symbol
      if(rsym->reserved() & 0x6u)
        return;
      if(!m_pGOT)
        createARMGOT(pLinker, pOutput);
      m_pGOT->reserveEntry();
      // If building shared object, a dynamic relocation with
      // type RELATIVE is needed to relocate this GOT entry.
      // Reserve an entry in .rel.dyn
      if(Output::DynObj == pOutput.type()) {
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createARMRelDyn(pLinker, pOutput);
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
}

void ARMGNULDBackend::scanGlobalReloc(Relocation& pReloc,
                                      MCLinker& pLinker,
                                      const MCLDInfo& pLDInfo,
                                      const Output& pOutput)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();

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
      if(isSymbolNeedsPLT(*rsym, pOutput, pLDInfo)) {
        // create plt for this symbol if it does not have one
        if(!(rsym->reserved() & 0x8u)){
          // create .plt and .rel.plt if not exist
          if(!m_pPLT)
            createARMPLTandRelPLT(pLinker, pOutput);
          // Symbol needs PLT entry, we need to reserve a PLT entry
          // and the corresponding GOT and dynamic relocation entry
          // in .got and .rel.plt. (GOT entry will be reserved simultaneously
          // when calling ARMPLT->reserveEntry())
          m_pPLT->reserveEntry();
          m_pRelPLT->reserveEntry(*m_pRelocFactory);
          // set PLT bit
          rsym->setReserved(rsym->reserved() | 0x8u);
        }
      }

      if(isSymbolNeedsDynRel(*rsym, pOutput, true)) {
        checkValidReloc(pReloc, pLDInfo, pOutput);
        // symbol needs dynamic relocation entry, reserve an entry in .rel.dyn
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createARMRelDyn(pLinker, pOutput);
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
        createARMGOT(pLinker, pOutput);
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
      if(isSymbolNeedsDynRel(*rsym, pOutput, false)) {
        checkValidReloc(pReloc, pLDInfo, pOutput);
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createARMRelDyn(pLinker, pOutput);
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

      // if symbol is defined in the ouput file and it's not
      // preemptible, no need plt
      if(rsym->isDefine() && !rsym->isDyn() &&
         !isSymbolPreemptible(*rsym, pOutput, pLDInfo)) {
        return;
      }

      // create .plt and .rel.plt if not exist
      if(!m_pPLT)
         createARMPLTandRelPLT(pLinker, pOutput);
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
        createARMGOT(pLinker, pOutput);
      m_pGOT->reserveEntry();
      // If building shared object or the symbol is undefined, a dynamic
      // relocation is needed to relocate this GOT entry. Reserve an
      // entry in .rel.dyn
      if(Output::DynObj == pOutput.type() || rsym->isUndef() || rsym->isDyn()) {
        // create .rel.dyn section if not exist
        if(!m_pRelDyn)
          createARMRelDyn(pLinker, pOutput);
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
}

void ARMGNULDBackend::scanRelocation(Relocation& pReloc,
                                     MCLinker& pLinker,
                                     const MCLDInfo& pLDInfo,
                                     const Output& pOutput)
{
  // rsym - The relocation target symbol
  ResolveInfo* rsym = pReloc.symInfo();
  assert(0 != rsym && "ResolveInfo of relocation not set while scanRelocation");

  // Scan relocation type to determine if an GOT/PLT/Dynamic Relocation
  // entries should be created.
  // FIXME: Below judgements concern only .so is generated as output
  // FIXME: Below judgements concern nothing about TLS related relocation

  // A refernece to symbol _GLOBAL_OFFSET_TABLE_ implies that a .got section
  // is needed
  if((NULL == m_pGOT) && (0 == strcmp(rsym->name(), "_GLOBAL_OFFSET_TABLE_"))) {
    createARMGOT(pLinker, pOutput);
  }

  // rsym is local
  if(rsym->isLocal())
    scanLocalReloc(pReloc, pLinker, pLDInfo, pOutput);

  // rsym is global
  else if(rsym->isGlobal())
    scanGlobalReloc(pReloc, pLinker, pLDInfo, pOutput);

}

uint64_t ARMGNULDBackend::emitSectionData(const Output& pOutput,
                                          const LDSection& pSection,
                                          const MCLDInfo& pInfo,
                                          MemoryRegion& pRegion) const
{
  assert(pRegion.size() && "Size of MemoryRegion is zero!");

  ELFFileFormat* file_format = NULL;
  switch (pOutput.type()) {
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    case Output::Object:
      break;
    default:
      llvm::report_fatal_error(llvm::Twine("Unsupported output file format: ") +
                               llvm::Twine(pOutput.type()));
      return 0x0;
  } // end of switch

  unsigned int entry_size = 0;
  uint64_t region_size = 0x0;

  if (&pSection == m_pAttributes) {
    // FIXME: Currently Emitting .ARM.attributes directly from the input file.
    const llvm::MCSectionData* sect_data = pSection.getSectionData();
    assert(sect_data &&
           "Emit .ARM.attribute failed, MCSectionData doesn't exist!");

    uint8_t* start =
              llvm::cast<MCRegionFragment>(
                     sect_data->getFragmentList().front()).getRegion().start();

    memcpy(pRegion.start(), start, pRegion.size());
  }

  else if (&pSection == &(file_format->getPLT())) {
    assert(NULL != m_pPLT && "emitSectionData failed, m_pPLT is NULL!");
    region_size = m_pPLT->emit(pRegion);
  }

  else if (&pSection == &(file_format->getGOT())) {
    assert(m_pGOT && "emitSectionData failed, m_pGOT is NULL!");

    uint32_t* buffer = reinterpret_cast<uint32_t*>(pRegion.getBuffer());

    GOTEntry* got = 0;
    entry_size = m_pGOT->getEntrySize();

    for (ARMGOT::iterator it = m_pGOT->begin(),
         ie = m_pGOT->end(); it != ie; ++it, ++buffer) {
      got = &(llvm::cast<GOTEntry>((*it)));
      *buffer = static_cast<uint32_t>(got->getContent());
      region_size += entry_size;
    }
  }

  else {
    llvm::report_fatal_error("unsupported section name "
                             + pSection.name() + " !");
    return 0x0;
  }

  pRegion.sync();

  return region_size;
}

/// finalizeSymbol - finalize the symbol value
/// If the symbol's reserved field is not zero, MCLinker will call back this
/// function to ask the final value of the symbol
bool ARMGNULDBackend::finalizeSymbol(LDSymbol& pSymbol) const
{
  return false;
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

OutputRelocSection& ARMGNULDBackend::getRelDyn()
{
  assert(0 != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

const OutputRelocSection& ARMGNULDBackend::getRelDyn() const
{
  assert(0 != m_pRelDyn && ".rel.dyn section not exist");
  return *m_pRelDyn;
}

OutputRelocSection& ARMGNULDBackend::getRelPLT()
{
  assert(0 != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

const OutputRelocSection& ARMGNULDBackend::getRelPLT() const
{
  assert(0 != m_pRelPLT && ".rel.plt section not exist");
  return *m_pRelPLT;
}

unsigned int
ARMGNULDBackend::getTargetSectionOrder(const Output& pOutput,
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

  if (&pSectHdr == &file_format->getGOT())
    return SHO_DATA;

  if (&pSectHdr == &file_format->getPLT())
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

