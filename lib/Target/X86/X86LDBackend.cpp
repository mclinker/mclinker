//===- X86LDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86.h"
#include "X86LDBackend.h"
#include "X86RelocationFactory.h"
#include <llvm/ADT/Triple.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/LD/LDSection.h>
#include <mcld/MC/MCLinker.h>

using namespace mcld;

X86GNULDBackend::X86GNULDBackend()
  : m_pRelocFactory(0), m_pGOT(0) {
}

X86GNULDBackend::~X86GNULDBackend()
{
  if (0 != m_pRelocFactory)
    delete m_pRelocFactory;
  if (0 != m_pGOT)
    delete m_pGOT;
}

RelocationFactory* X86GNULDBackend::getRelocFactory()
{
  if (0 == m_pRelocFactory)
    m_pRelocFactory = new X86RelocationFactory(1024, *this);
  return m_pRelocFactory;
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

unsigned int X86GNULDBackend::bitclass() const
{
  return 32;
}

void X86GNULDBackend::initTargetSections(MCLinker& pLinker, LDContext& pContext)
{
  const LDSection& got = pContext.getOrCreateSection(".got",
                                                     LDFileFormat::GOT,
                                                     ELF::SHT_PROGBITS,
                                                     ELF::SHF_ALLOC | ELF::SHF_WRITE);
  m_pGOT = new X86GOT(got);
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
