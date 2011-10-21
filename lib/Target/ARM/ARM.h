//===- ARM.h --------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_H
#define MCLD_ARM_H
#include <string>
#include "mcld/Target/TargetMachine.h"

namespace mcld {
class TargetLDBackend;

extern mcld::Target TheARMTarget;

TargetLDBackend *createARMLDBackend(const llvm::Target&, const std::string&);

} // namespace of mcld

#endif

