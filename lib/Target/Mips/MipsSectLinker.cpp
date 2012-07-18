//===- MipsSectLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Triple.h>
#include <mcld/Support/TargetRegistry.h>
#include "Mips.h"
#include "MipsELFSectLinker.h"

using namespace mcld;

namespace mcld {
//===----------------------------------------------------------------------===//
/// createMipsSectLinker - the help funtion to create
/// corresponding MipsSectLinker
///
SectLinker* createMipsSectLinker(const std::string &pTriple,
                                 SectLinkerOption &pOption,
                                 mcld::TargetLDBackend &pLDBackend)
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

  return new MipsELFSectLinker(pOption, pLDBackend);
}

} // namespace of mcld

//==========================
// MipsSectLinker
extern "C" void LLVMInitializeMipsSectLinker() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterSectLinker(TheMipselTarget,
                                           createMipsSectLinker);
}
