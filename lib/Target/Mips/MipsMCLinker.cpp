//===- MipsMCLinker.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "Mips.h"
#include "MipsELFMCLinker.h"
#include <llvm/ADT/Triple.h>
#include <mcld/Module.h>
#include <mcld/Support/TargetRegistry.h>

namespace {

//===----------------------------------------------------------------------===//
/// createMipsMCLinker - the help funtion to create corresponding MipsMCLinker
//===----------------------------------------------------------------------===//
mcld::MCLinker* createMipsMCLinker(const std::string &pTriple,
                                   mcld::LinkerConfig& pConfig,
                                   mcld::Module& pModule,
                                   mcld::FileHandle& pFileHandle)
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

  return new mcld::MipsELFMCLinker(pConfig, pModule, pFileHandle);
}

} // namespace of mcld

//===----------------------------------------------------------------------===//
// MipsMCLinker
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeMipsMCLinker() {
  mcld::TargetRegistry::RegisterMCLinker(mcld::TheMipselTarget,
                                         createMipsMCLinker);
  mcld::TargetRegistry::RegisterMCLinker(mcld::TheMips64elTarget,
                                         createMipsMCLinker);
}
