//===- MipsTargetMachine.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MipsTargetMachine.h"
#include "mcld/Target/TargetMachine.h"
#include "mcld/Support/TargetRegistry.h"
#include "mcld/MC/MCLDInfo.h"
#include "Mips.h"

extern "C" void LLVMInitializeMipsLDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::MipsBaseTargetMachine>
        X(mcld::TheMipselTarget);
}

mcld::MipsBaseTargetMachine::MipsBaseTargetMachine(llvm::TargetMachine& pPM,
                                                   const mcld::Target &pTarget,
                                                   const std::string& pTriple)
  : mcld::LLVMTargetMachine(pPM, pTarget, pTriple) {
  // arg1 - the number of total attributes
  // arg2 - the most possible number of input files
  m_pLDInfo = new MCLDInfo(pTriple, 32, 64);
}

mcld::MipsBaseTargetMachine::~MipsBaseTargetMachine()
{
  delete m_pLDInfo;
}
