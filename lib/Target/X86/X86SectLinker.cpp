//===- X86SectLinker.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/ADT/Triple.h>
#include <mcld/Support/TargetRegistry.h>

#include "X86.h"
#include "X86ELFSectLinker.h"

using namespace mcld;

namespace mcld {
//===----------------------------------------------------------------------===//
/// createX86SectLinker - the help funtion to create corresponding X86SectLinker
///
SectLinker* createX86SectLinker(const std::string &pTriple,
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

  if (theTriple.isArch32Bit())
    return new X86ELFSectLinker(pOption, pLDBackend);

  assert(0 && "X86_64 has not supported yet");
  return NULL;
}

} // namespace of mcld

//==========================
// X86SectLinker
extern "C" void LLVMInitializeX86SectLinker() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterSectLinker(TheX86Target, createX86SectLinker);
}

