//===- ARMMCLinker.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/ADT/Triple.h>
#include <mcld/Support/TargetRegistry.h>
#include "ARM.h"
#include "ARMELFMCLinker.h"


using namespace mcld;

namespace mcld {
//===----------------------------------------------------------------------===//
// createARMMCLinker - the help function to create corresponding ARMMCLinker
//===----------------------------------------------------------------------===//
MCLinker* createARMMCLinker(const std::string &pTriple,
                            SectLinkerOption &pOption,
                            mcld::TargetLDBackend &pLDBackend)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker has not supported yet");
    return NULL;
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker has not supported yet");
    return NULL;
  }

  // For now, use Android MCLinker directly
  return new ARMELFMCLinker(pOption, pLDBackend);
}

} // namespace of mcld

//===----------------------------------------------------------------------===//
// ARMMCLinker
//===----------------------------------------------------------------------===//
extern "C" void LLVMInitializeARMMCLinker() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterMCLinker(TheARMTarget, createARMMCLinker);
  mcld::TargetRegistry::RegisterMCLinker(TheThumbTarget, createARMMCLinker);
}

