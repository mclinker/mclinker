//===- X86SectLinker.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "llvm/ADT/Triple.h"
#include "mcld/CodeGen/SectLinker.h"
#include "mcld/Support/TargetRegistry.h"
#include "mcld/MC/MCLDInfo.h"
#include "X86.h"
#include "X86AndroidSectLinker.h"
#include "X86ELFSectLinker.h"

#include "mcld/CodeGen/SectLinker.h"
#include "mcld/MC/MCLDInfo.h"
#include "mcld/Support/TargetRegistry.h"
#include "llvm/ADT/Triple.h"

using namespace mcld;

namespace mcld {
//===----------------------------------------------------------------------===//
/// createX86SectLinker - the help funtion to create corresponding X86SectLinker
///
SectLinker* createX86SectLinker(const std::string &pTriple,
                                const std::string &pInputFilename,
                                const std::string &pOutputFilename,
                                unsigned int pOutputLinkType,
                                MCLDInfo& pLDInfo,
                                mcld::TargetLDBackend &pLDBackend)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker has not supported yet");
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker has not supported yet");
  }

  // For now, use Android SectLinker directly
  return new X86AndroidSectLinker(pInputFilename,
                                  pOutputFilename,
                                  pOutputLinkType,
                                  pLDInfo,
                                  pLDBackend);
}

} // namespace of mcld

//==========================
// X86SectLinker
extern "C" void LLVMInitializeX86SectLinker() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterSectLinker(TheX86Target, createX86SectLinker);
}

