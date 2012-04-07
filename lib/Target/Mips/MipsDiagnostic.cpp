//===- MipsDiagnostic.cpp -------------------------------------------------===//
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
#include "MipsDiagnostic.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// MipsDiagnostic


namespace mcld {
//===----------------------------------------------------------------------===//
// createMipsDiagnostic - the help function to create corresponding
// MipsDiagnostic
LDDiagnostic* createMipsDiagnostic(const llvm::Target& pTarget,
                                  const std::string &pTriple)
{
  return new MipsDiagnostic();
}

} // namespace of mcld

//==========================
// InitializeMipsDiagnostic
extern "C" void LLVMInitializeMipsDiagnostic() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterDiagnostic(TheMipselTarget, createMipsDiagnostic);
}

