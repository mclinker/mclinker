//===- ARM.h --------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_ARM_ARM_H
#define TARGET_ARM_ARM_H
#include <string>

namespace llvm {
class Target;
} // namespace of llvm

namespace mcld {

class Target;
class TargetLDBackend;

extern mcld::Target TheARMTarget;
extern mcld::Target TheThumbTarget;

TargetLDBackend *createARMLDBackend(const llvm::Target&, const std::string&);

} // namespace of mcld

#endif

