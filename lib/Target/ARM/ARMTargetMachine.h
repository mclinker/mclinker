//===- ARMTargetMachine.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_ARM_ARMTARGETMACHINE_H
#define TARGET_ARM_ARMTARGETMACHINE_H

#include "ARM.h"
#include <mcld/CodeGen/TargetMachine.h>

namespace mcld {

class ARMBaseTargetMachine : public MCLDTargetMachine
{
public:
  ARMBaseTargetMachine(llvm::TargetMachine& pTM,
                       const llvm::Target& pLLVMTarget,
                       const mcld::Target& pMCLDTarget,
                       const std::string& pTriple);
};

} // namespace of mcld

#endif

