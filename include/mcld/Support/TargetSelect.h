//===- TargetSelect.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_SELECT_H
#define MCLD_TARGET_SELECT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

extern "C" {
  // Declare all of the target-initialization functions that are available.
#define LLVM_TARGET(TargetName) void LLVMInitialize##TargetName##LDTargetInfo();
#include "mcld/Config/Targets.def"

  // Declare all of the target-dependent functions that are available.
#define LLVM_TARGET(TargetName) void LLVMInitialize##TargetName##LDTarget();
#include "mcld/Config/Targets.def"

  // Declare all of the target-depedent linker information
#define LLVM_LINKER(TargetName) void LLVMInitialize##TargetName##LDInfo();
#include "mcld/Config/Linkers.def"

  // Declare all of the available linker environment.
#define LLVM_LINKER(TargetName) void LLVMInitialize##TargetName##SectLinker();
#include "mcld/Config/Linkers.def"

  // Declare all of the available target-specific linker
#define LLVM_LINKER(TargetName) void LLVMInitialize##TargetName##LDBackend();
#include "mcld/Config/Linkers.def"

  // Declare all of the available target-specific diagnostic line infomation
#define LLVM_LINKER(TargetName) void LLVMInitialize##TargetName##DiagnosticLineInfo();
#include "mcld/Config/Linkers.def"

} // extern "C"

namespace mcld
{
  /// InitializeAllTargetInfos - The main program should call this function if
  /// it wants access to all available targets that LLVM is configured to
  /// support, to make them available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllTargetInfos() {
#define LLVM_TARGET(TargetName) LLVMInitialize##TargetName##LDTargetInfo();
#include "mcld/Config/Targets.def"
  }

  /// InitializeAllTargets - The main program should call this function if it
  /// wants access to all available target machines that LLVM is configured to
  /// support, to make them available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllTargets() {
    mcld::InitializeAllTargetInfos();

#define LLVM_TARGET(TargetName) LLVMInitialize##TargetName##LDTarget();
#include "mcld/Config/Targets.def"

#define LLVM_TARGET(TargetName) LLVMInitialize##TargetName##LDBackend();
#include "mcld/Config/Targets.def"
  }

  /// InitializeAllLinkers - The main program should call this function if it
  /// wants all linkers that LLVM is configured to support, to make them
  /// available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllLinkers() {
#define LLVM_LINKER(TargetName) LLVMInitialize##TargetName##SectLinker();
#include "mcld/Config/Linkers.def"
  }

  /// InitializeMsgHandler - The main program should call this function if it
  /// wants to print linker-specific messages. To make them available via the
  /// TargetRegistry.
  inline void InitializeAllDiagnostics() {
#define LLVM_LINKER(TargetName)  LLVMInitialize##TargetName##DiagnosticLineInfo();
#include "mcld/Config/Linkers.def"
  }

} // namespace of mcld

#endif

