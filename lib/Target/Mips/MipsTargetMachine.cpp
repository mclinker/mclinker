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
#include <mcld/Target/TargetMachine.h>
#include <mcld/Support/TargetRegistry.h>

typedef mcld::RegisterTargetMachine<mcld::MipsBaseTargetMachine> RegMipsTarget;

extern "C" void MCLDInitializeMipsLDTarget() {
  RegMipsTarget X1(mcld::TheMipselTarget);
  RegMipsTarget X2(mcld::TheMips64elTarget);
}

mcld::MipsBaseTargetMachine::MipsBaseTargetMachine(llvm::TargetMachine& pPM,
                                                   const mcld::Target &pTarget,
                                                   const std::string& pTriple)
  : mcld::MCLDTargetMachine(pPM, pTarget, pTriple) {
}
