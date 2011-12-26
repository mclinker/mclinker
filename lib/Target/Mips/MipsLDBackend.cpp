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
#include "MipsLDBackend.h"
#include "MipsRelocationFactory.h"

using namespace llvm;
using namespace mcld;

MipsGNULDBackend::MipsGNULDBackend()
  : m_pRelocFactory(0), m_pGOT(0) {
}

MipsGNULDBackend::~MipsGNULDBackend()
{
  if (0 != m_pRelocFactory)
    delete m_pRelocFactory;
  if (0 != m_pGOT)
    delete m_pGOT;
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
  LDSection& sec = pLinker.createSectHdr(".got",
                                         LDFileFormat::Target,
                                         ELF::SHT_PROGBITS,
                                         ELF::SHF_ALLOC | ELF::SHF_WRITE);

  llvm::MCSectionData& data = pLinker.getOrCreateSectData(sec);

  m_pGOT.reset(new MipsGOT(data));

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
  switch (pReloc.type()){
    case ELF::R_MIPS_NONE:
      break;
    case ELF::R_MIPS_32:
      // TODO: (simon) Reserve .rel.dyn entry
      break;
    case ELF::R_MIPS_HI16:
    case ELF::R_MIPS_LO16:
      break;
    case ELF::R_MIPS_GOT16:
    case ELF::R_MIPS_CALL16:
      // TODO: (simon) Reserve .got entry
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
      // TODO: (simon) Reserve .rel.dyn entry
      break;
    case ELF::R_MIPS_GOT16:
    case ELF::R_MIPS_CALL16:
      // TODO: (simon) Reserve .got entry
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

MipsGOT& MipsGNULDBackend::getGOT()
{
  assert(0 != m_pGOT);
  return *m_pGOT;
}

const MipsGOT& MipsGNULDBackend::getGOT() const
{
  assert(0 != m_pGOT);
  return *m_pGOT;
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
