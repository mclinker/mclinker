//===- X86TargetInfo.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/TargetMachine.h"
#include "mcld/Support/TargetRegistry.h"

namespace mcld {

mcld::Target TheX86Target;

extern "C" void LLVMInitializeX86LDTargetInfo() {
  // register into mcld::TargetRegistry
  mcld::RegisterTarget X(TheX86Target, "x86");
}

} // namespace of mcld

