//===- AArch64.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_AARCH64_AARCH64_H
#define TARGET_AARCH64_AARCH64_H
#include <string>

namespace llvm {
class Target;
} // namespace of llvm

namespace mcld {

class Target;
class TargetLDBackend;

extern mcld::Target TheAArch64Target;

TargetLDBackend *createAArch64LDBackend(const llvm::Target&,
                                        const std::string&);

} // namespace of mcld

#endif

