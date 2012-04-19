//===- ARMDiagnostic.cpp --------------------------------------------------===//
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
#include "ARMDiagnostic.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// ARMDiagnostic


namespace mcld {
//===----------------------------------------------------------------------===//
// createARMDiagnostic - the help function to create corresponding ARMDiagnostic
//
LDDiagnostic* createARMDiagnostic(const llvm::Target& pTarget,
                                  const std::string &pTriple)
{
  return new ARMDiagnostic();
}

} // namespace of mcld

//==========================
// InitializeARMDiagnostic
extern "C" void LLVMInitializeARMDiagnostic() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterDiagnostic(TheARMTarget, createARMDiagnostic);
  mcld::TargetRegistry::RegisterDiagnostic(TheThumbTarget, createARMDiagnostic);
}

