//===- X86TargetMachine.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_X86_X86TARGETMACHINE_H
#define TARGET_X86_X86TARGETMACHINE_H
#include "X86.h"
#include <mcld/CodeGen/TargetMachine.h>

namespace mcld {

class X86TargetMachine : public MCLDTargetMachine
{
public:
  X86TargetMachine(llvm::TargetMachine& pTM,
                   const llvm::Target& pLLVMTarget,
                   const mcld::Target& pMCLDTarget,
                   const std::string& pTriple);
};

} // namespace of mcld

#endif

