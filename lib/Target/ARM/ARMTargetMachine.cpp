//===- ARMTargetMachine.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMTargetMachine.h"

#include "mcld/Support/TargetRegistry.h"
#include "mcld/MC/MCLDInfo.h"
#include "ARM.h"

extern "C" void LLVMInitializeARMLDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::ARMBaseTargetMachine> X(mcld::TheARMTarget);
  mcld::RegisterTargetMachine<mcld::ARMBaseTargetMachine> Y(mcld::TheThumbTarget);
}

mcld::ARMBaseTargetMachine::ARMBaseTargetMachine(llvm::TargetMachine& pPM,
                                                 const mcld::Target &pTarget,
                                                 const std::string& pTriple)
  : mcld::LLVMTargetMachine(pPM, pTarget, pTriple) {
  // arg1 - the number of total attributes
  // arg2 - the most possible number of input files
  m_pLDInfo = new MCLDInfo(pTriple, 32, 64);
}

mcld::ARMBaseTargetMachine::~ARMBaseTargetMachine()
{
  delete m_pLDInfo;
}

