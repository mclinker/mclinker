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
#include <mcld/LinkerConfig.h>

extern "C" void MCLDInitializeMipsLDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::MipsBaseTargetMachine>
        X(mcld::TheMipselTarget);
}

mcld::MipsBaseTargetMachine::MipsBaseTargetMachine(llvm::TargetMachine& pPM,
                                                   const mcld::Target &pTarget,
                                                   const std::string& pTriple)
  : mcld::MCLDTargetMachine(pPM, pTarget, pTriple) {
  m_pConfig = new LinkerConfig(pTriple);
}

mcld::MipsBaseTargetMachine::~MipsBaseTargetMachine()
{
  delete m_pConfig;
}
