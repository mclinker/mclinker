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

void MipsGNULDBackend::initTargetSections(MCLinker& pLinker)
{
  const LDSection* got = pLinker.getOrCreateSection(".got",
                                                    LDFileFormat::GOT,
                                                    ELF::SHT_PROGBITS,
                                                    ELF::SHF_ALLOC | ELF::SHF_WRITE);
  m_pGOT = new MipsGOT(*got);

  // add target dependent sections here.
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
