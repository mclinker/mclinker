//===- MipsLDBackend.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Triple.h>
#include "mcld/Support/TargetRegistry.h"
#include "Mips.h"
#include "MipsLDBackend.h"

using namespace mcld;

MipsGNULDBackend::MipsGNULDBackend()
  : m_pRelocFactory(0) {
}

MipsGNULDBackend::~MipsGNULDBackend()
{
}

MipsRelocationFactory* MipsGNULDBackend::getRelocFactory()
{
  if (NULL == m_pRelocFactory)
    m_pRelocFactory = new MipsRelocationFactory(1024);
  return m_pRelocFactory;
}

uint32_t MipsGNULDBackend::machine() const
{
  return EM_MIPS;
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
