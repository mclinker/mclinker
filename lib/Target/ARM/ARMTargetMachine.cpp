/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Target/TargetMachine.h>
#include <mcld/Target/TargetRegistry.h>
#include "ARM.h"
#include "ARMTargetMachine.h"

extern "C" void LLVMInitializeARMLDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::ARMBaseTargetMachine> X(mcld::TheARMTarget);
}

mcld::ARMBaseTargetMachine::ARMBaseTargetMachine(llvm::TargetMachine& pPM,
                                                 const mcld::Target &pTarget,
                                                 const std::string& pTriple)
  : mcld::LLVMTargetMachine(pPM, pTarget, pTriple) {
  // arg1 - the number of total attributes
  // arg2 - the most possible number of input files
  m_pLDInfo = new MCLDInfo( 32, 64);
}

mcld::ARMBaseTargetMachine::~ARMBaseTargetMachine()
{
  delete m_pLDInfo;
}

