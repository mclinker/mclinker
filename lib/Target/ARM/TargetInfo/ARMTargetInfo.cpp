//===- ARMTargetInfo.cpp --------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/TargetMachine.h"
#include "mcld/Target/TargetRegistry.h"

namespace mcld {

mcld::Target TheARMTarget;

extern "C" void LLVMInitializeARMLDTargetInfo() {
  // register into mcld::TargetRegistry
  mcld::RegisterTarget X(TheARMTarget, "arm" );
}

} // namespace of mcld

