//===- AArch64TargetMachine.h ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_AARCH64_TARGET_MACHINE_H
#define MCLD_AARCH64_TARGET_MACHINE_H

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

