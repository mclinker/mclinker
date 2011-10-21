//===- X86TargetMachine.cpp -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/TargetMachine.h"
#include "mcld/Support/TargetRegistry.h"
#include "X86.h"
#include "X86TargetMachine.h"

extern "C" void LLVMInitializeX86LDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::X86TargetMachine> X(mcld::TheX86Target);
}

mcld::X86TargetMachine::X86TargetMachine(llvm::TargetMachine& pPM,
                                         const mcld::Target &pTarget,
                                         const std::string& pTriple)
  : mcld::LLVMTargetMachine(pPM, pTarget, pTriple) {
  // arg1 - the number of total attributes
  // arg2 - the most possible number of input files
  m_pLDInfo = new MCLDInfo(pTriple, 32, 64);
}

mcld::X86TargetMachine::~X86TargetMachine()
{
  delete m_pLDInfo;
}

