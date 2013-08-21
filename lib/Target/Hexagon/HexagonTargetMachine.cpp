//===- HexagonTargetMachine.cpp -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "HexagonTargetMachine.h"
#include "Hexagon.h"
#include <mcld/Support/TargetRegistry.h>

extern "C" void MCLDInitializeHexagonLDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::HexagonTargetMachine>
     X(mcld::TheHexagonTarget);
}

using namespace mcld;

//===----------------------------------------------------------------------===//
// HexagonTargetMachine
//===----------------------------------------------------------------------===//
HexagonTargetMachine::HexagonTargetMachine(llvm::TargetMachine& pPM,
                                           const llvm::Target& pLLVMTarget,
                                           const mcld::Target& pMCLDTarget,
                                           const std::string& pTriple)
  : MCLDTargetMachine(pPM, pLLVMTarget, pMCLDTarget, pTriple) {
}
