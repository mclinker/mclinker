//===- ARMTargetMachine.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMTargetMachine.h"
#include "ARM.h"

#include <mcld/Support/TargetRegistry.h>

using namespace mcld;

ARMBaseTargetMachine::ARMBaseTargetMachine(llvm::TargetMachine& pPM,
                                           const llvm::Target &pLLVMTarget,
                                           const mcld::Target &pMCLDTarget,
                                           const std::string& pTriple)
  : MCLDTargetMachine(pPM, pLLVMTarget, pMCLDTarget, pTriple) {
}

//===----------------------------------------------------------------------===//
// Initialize MCLDTargetMachine
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeARMLDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::ARMBaseTargetMachine> X(mcld::TheARMTarget);
  mcld::RegisterTargetMachine<mcld::ARMBaseTargetMachine> Y(mcld::TheThumbTarget);
}

