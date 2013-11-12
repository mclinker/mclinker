//===- AArch64TargetInfo.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Support/Target.h>

namespace mcld {

mcld::Target TheAArch64Target;

extern "C" void MCLDInitializeAArch64LDTargetInfo() {
  // register into mcld::TargetRegistry
  mcld::RegisterTarget<llvm::Triple::aarch64> X(TheAArch64Target, "aarch64");
}

} // namespace of mcld

