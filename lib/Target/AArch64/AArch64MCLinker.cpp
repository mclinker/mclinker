//===- AArch64MCLinker.cpp-------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "AArch64ELFMCLinker.h"

#include "AArch64.h"
#include <llvm/ADT/Triple.h>
#include <mcld/Module.h>
#include <mcld/Support/TargetRegistry.h>

using namespace mcld;

namespace mcld {
//===----------------------------------------------------------------------===//
// createAArch64MCLinker - the help function to create corresponding
// AArch64MCLinker
//===----------------------------------------------------------------------===//
MCLinker* createAArch64MCLinker(const std::string& pTriple,
                                LinkerConfig& pConfig,
                                mcld::Module& pModule,
                                FileHandle& pFileHandle)
{
  llvm::Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker has not supported yet");
    return NULL;
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker has not supported yet");
    return NULL;
  }

  return new AArch64ELFMCLinker(pConfig, pModule, pFileHandle);
}

} // namespace of mcld

//===----------------------------------------------------------------------===//
// AArch64MCLinker
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeAArch64MCLinker() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterMCLinker(TheAArch64Target,
                                         createAArch64MCLinker);
}

