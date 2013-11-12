//===- AArch64TargetMachine.cpp -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "AArch64TargetMachine.h"
#include "AArch64.h"

#include <mcld/Support/TargetRegistry.h>

using namespace mcld;

AArch64BaseTargetMachine::AArch64BaseTargetMachine(llvm::TargetMachine& pPM,
                                                const llvm::Target &pLLVMTarget,
                                                const mcld::Target &pMCLDTarget,
                                                const std::string& pTriple)
  : MCLDTargetMachine(pPM, pLLVMTarget, pMCLDTarget, pTriple) {
}

//===----------------------------------------------------------------------===//
// Initialize MCLDTargetMachine
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeAArch64LDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::AArch64BaseTargetMachine> X(mcld::TheAArch64Target);
}

