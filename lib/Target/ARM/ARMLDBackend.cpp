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
#include "mcld/Support/TargetRegistry.h"
#include <llvm/ADT/Triple.h>

using namespace mcld;

ARMGNULDBackend::ARMGNULDBackend()
  : m_pRelocFactory(0) {
}

ARMGNULDBackend::~ARMGNULDBackend()
{
}

ARMRelocationFactory* ARMGNULDBackend::getRelocFactory()
{
  if (NULL == m_pRelocFactory)
    m_pRelocFactory = new ARMRelocationFactory(1024);
  return m_pRelocFactory;
}

uint32_t ARMGNULDBackend::machine() const
{
  return EM_ARM;
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
