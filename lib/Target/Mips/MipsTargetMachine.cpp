//===- MipsTargetMachine.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "MipsTargetMachine.h"
#include "Mips.h"
#include <mcld/Support/TargetRegistry.h>

typedef mcld::RegisterTargetMachine<mcld::MipsBaseTargetMachine> RegMipsTarget;

extern "C" void MCLDInitializeMipsLDTarget() {
  RegMipsTarget X1(mcld::TheMipselTarget);
  RegMipsTarget X2(mcld::TheMips64elTarget);
}

using namespace mcld;

//===----------------------------------------------------------------------===//
// MipsBaseTargetMachine
//===----------------------------------------------------------------------===//
MipsBaseTargetMachine::MipsBaseTargetMachine(llvm::TargetMachine& pPM,
                                             const llvm::Target &pLLVMTarget,
                                             const mcld::Target &pMCLDTarget,
                                             const std::string& pTriple)
  : MCLDTargetMachine(pPM, pLLVMTarget, pMCLDTarget, pTriple) {
}
