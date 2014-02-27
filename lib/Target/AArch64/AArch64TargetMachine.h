//===- AArch64TargetMachine.h ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_AARCH64_AARCH64TARGETMACHINE_H
#define TARGET_AARCH64_AARCH64TARGETMACHINE_H

#include "AArch64.h"
#include <mcld/CodeGen/TargetMachine.h>

namespace mcld {

class AArch64BaseTargetMachine : public MCLDTargetMachine
{
public:
  AArch64BaseTargetMachine(llvm::TargetMachine& pTM,
                           const llvm::Target& pLLVMTarget,
                           const mcld::Target& pMCLDTarget,
                           const std::string& pTriple);
};

} // namespace of mcld

#endif

