//===- X86TargetMachine.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86TargetMachine.h"

#include "X86.h"
#include <mcld/Target/TargetMachine.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/LinkerConfig.h>

extern "C" void LLVMInitializeX86LDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::X86TargetMachine> X(mcld::TheX86Target);
}

mcld::X86TargetMachine::X86TargetMachine(llvm::TargetMachine& pPM,
                                         const mcld::Target &pTarget,
                                         const std::string& pTriple)
  : mcld::LLVMTargetMachine(pPM, pTarget, pTriple) {
  // arg2 - the most possible number of input files
  m_pConfig = new LinkerConfig(pTriple, 64);
}

mcld::X86TargetMachine::~X86TargetMachine()
{
  delete m_pConfig;
}

