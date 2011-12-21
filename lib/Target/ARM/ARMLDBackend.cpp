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
#include <mcld/Support/TargetRegistry.h>
#include <mcld/MC/MCLinker.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Support/ELF.h>

using namespace mcld;

ARMGNULDBackend::ARMGNULDBackend()
  : m_pRelocFactory(0), m_pGOT(0) {
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

void ARMGNULDBackend::initTargetSections(MCLinker& pLinker)
{
  const LDSection* got = pLinker.getOrCreateSectHdr(".got",
                                                    LDFileFormat::GOT,
                                                    ELF::SHT_PROGBITS,
                                                    ELF::SHF_ALLOC | ELF::SHF_WRITE);
  assert(NULL != got);
  m_pGOT = new ARMGOT(*got);

  const LDSection* plt = pLinker.getOrCreateSectHdr(".plt",
                                                    LDFileFormat::PLT,
                                                    ELF::SHT_PROGBITS,
                                                    ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);           
  assert(NULL != plt);
  m_pPLT = new ARMPLT(*plt);

  
  const LDSection* reldyn = pLinker.getOrCreateSectHdr(".rel.dyn",
                                                    LDFileFormat::Data,
                                                    ELF::SHT_REL,
                                                    ELF::SHF_ALLOC);           
  assert(NULL != reldyn);
  m_pRelDyn = new llvm::MCSectionData(*reldyn);

  const LDSection* relplt = pLinker.getOrCreateSectHdr(".rel.plt",
                                                    LDFileFormat::Data,
                                                    ELF::SHT_REL,
                                                    ELF::SHF_ALLOC);           
  assert(NULL != relplt);
  m_pRelPLT = new llvm::MCSectionData(*relplt);
  
  m_pEXIDX             = pLinker.getOrCreateSectHdr(".ARM.exidx",
                                                    LDFileFormat::MetaData,
                                                    ELF::SHT_ARM_EXIDX,
                                                    ELF::SHF_ALLOC | ELF::SHF_LINK_ORDER);
  m_pEXTAB             = pLinker.getOrCreateSectHdr(".ARM.extab",
                                                    LDFileFormat::MetaData,
                                                    ELF::SHT_PROGBITS,
                                                    ELF::SHF_ALLOC);
  m_pAttributes        = pLinker.getOrCreateSectHdr(".ARM.attributes",
                                                    LDFileFormat::MetaData,
                                                    ELF::SHT_ARM_ATTRIBUTES,
                                                    ELF::SHF_ALLOC);
}

void ARMGNULDBackend::scanRelocation(Relocation& pReloc)
{
}

ARMGOT& ARMGNULDBackend::getGOT()
{
  assert(0 != m_pGOT);
  return *m_pGOT;
}

const ARMGOT& ARMGNULDBackend::getGOT() const
{
  assert(0 != m_pGOT);
  return *m_pGOT;
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
