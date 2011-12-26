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
#include <mcld/Support/TargetRegistry.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/SectionMap.h>
#include "Mips.h"
#include "MipsDynRelSection.h"
#include "MipsLDBackend.h"
#include "MipsRelocationFactory.h"

using namespace llvm;
using namespace mcld;

MipsGNULDBackend::MipsGNULDBackend()
  : m_pRelocFactory(0) {
}

MipsGNULDBackend::~MipsGNULDBackend()
{
  if (0 != m_pRelocFactory)
    delete m_pRelocFactory;
}

RelocationFactory* MipsGNULDBackend::getRelocFactory()
{
  if (NULL == m_pRelocFactory)
    m_pRelocFactory = new MipsRelocationFactory(1024, *this);
  return m_pRelocFactory;
}

uint32_t MipsGNULDBackend::machine() const
{
  return ELF::EM_MIPS;
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

bool MipsGNULDBackend::initTargetSectionMap(SectionMap& pSectionMap)
{
  return true;
}

void MipsGNULDBackend::initTargetSections(MCLinker& pLinker)
{
  // add target dependent sections here.
}


void MipsGNULDBackend::scanRelocation(Relocation& pReloc,
                                      MCLinker& pLinker,
                                      unsigned int pType)
{
  ResolveInfo* rsym = pReloc.symInfo();

  if (rsym->isLocal())
    scanLocalRelocation(pReloc, pLinker, pType);
  else if (rsym->isGlobal())
    scanGlobalRelocation(pReloc, pLinker, pType);
}

void MipsGNULDBackend::scanLocalRelocation(Relocation& pReloc,
                                           MCLinker& pLinker,
                                           unsigned int pType)
{
  ResolveInfo* rsym = pReloc.symInfo();

  switch (pReloc.type()){
    case ELF::R_MIPS_NONE:
      break;
    case ELF::R_MIPS_32:
      if (Output::DynObj == pType) {
        // TODO: (simon) Check section flag SHF_EXECINSTR
        // half_t shndx = rsym->getSectionIndex();
        if (true) {
          if (NULL == m_pRelDynSec.get())
            createRelDynSec(pLinker);

          m_pRelDynSec->reserveEntry(*m_pRelocFactory);
        }
      }
      break;
    case ELF::R_MIPS_HI16:
    case ELF::R_MIPS_LO16:
      break;
    case ELF::R_MIPS_GOT16:
    case ELF::R_MIPS_CALL16:
      if (NULL == m_pGOT.get())
        createGOTSec(pLinker);

      m_pGOT->reserveEntry();
      break;
    case ELF::R_MIPS_GPREL32:
      break;
    default:
      llvm::report_fatal_error(llvm::Twine("Unknown relocation type. ") +
                               llvm::Twine("To symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
  }
}

void MipsGNULDBackend::scanGlobalRelocation(Relocation& pReloc,
                                            MCLinker& pLinker,
                                            unsigned int pType)
{
  ResolveInfo* rsym = pReloc.symInfo();

  switch (pReloc.type()){
    case ELF::R_MIPS_NONE:
      break;
    case ELF::R_MIPS_32:
    case ELF::R_MIPS_HI16:
    case ELF::R_MIPS_LO16:
      if (isSymbolNeedsDynRel(*rsym, pType)) {
        if (NULL == m_pRelDynSec.get())
          createRelDynSec(pLinker);

        m_pRelDynSec->reserveEntry(*m_pRelocFactory);
      }
      break;
    case ELF::R_MIPS_GOT16:
    case ELF::R_MIPS_CALL16:
      if (NULL == m_pGOT.get())
        createGOTSec(pLinker);

      m_pGOT->reserveEntry();
      break;
    case ELF::R_MIPS_GPREL32:
      llvm::report_fatal_error(llvm::Twine("R_MIPS_GPREL32 not defined for ") +
                               llvm::Twine("global symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
      break;
    default:
      llvm::report_fatal_error(llvm::Twine("Unknown relocation type. ") +
                               llvm::Twine("To symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
  }
}

void MipsGNULDBackend::createRelDynSec(MCLinker& pLinker)
{
  LDSection& sec = pLinker.createSectHdr(".rel.dyn",
                                         LDFileFormat::Relocation,
                                         ELF::SHT_REL,
                                         ELF::SHF_ALLOC);

  llvm::MCSectionData& data = pLinker.getOrCreateSectData(sec);

  m_pRelDynSec.reset(new MipsDynRelSection(data));
}

void MipsGNULDBackend::createGOTSec(MCLinker& pLinker)
{
  LDSection& sec = pLinker.createSectHdr(".got",
                                         LDFileFormat::Target,
                                         ELF::SHT_PROGBITS,
                                         ELF::SHF_ALLOC | ELF::SHF_WRITE);

  llvm::MCSectionData& data = pLinker.getOrCreateSectData(sec);

  m_pGOT.reset(new MipsGOT(data));
}

bool MipsGNULDBackend::isSymbolNeedsDynRel(ResolveInfo& pSym,
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

MipsGOT& MipsGNULDBackend::getGOT()
{
  assert(NULL != m_pGOT.get());
  return *m_pGOT.get();
}

const MipsGOT& MipsGNULDBackend::getGOT() const
{
  assert(NULL != m_pGOT.get());
  return *m_pGOT.get();
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createMipsLDBackend - the help funtion to create corresponding MipsLDBackend
///
static TargetLDBackend* createMipsLDBackend(const llvm::Target& pTarget,
                                            const std::string& pTriple)
{
  Triple theTriple(pTriple);
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
  mcld::TargetRegistry::RegisterTargetLDBackend(TheMipselTarget,
                                                createMipsLDBackend);
}
