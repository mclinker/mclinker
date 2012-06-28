//===- Initialization.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <cstdlib>
#include <string>

#include "alone/Config/Config.h"
#include "alone/Support/Initialization.h"

#include <mcld/Support/TargetSelect.h>
#include <mcld/Support/TargetRegistry.h>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ErrorHandling.h>

namespace {

void llvm_error_handler(void *pUserData, const std::string& pMessage) {
  ALOGE("%s", pMessage.c_str());
  ::exit(1);
}

} // end anonymous namespace

void alone::init::Initialize() {
  static bool is_initialized = false;
  if (is_initialized) {
    return;
  }

  // Setup error handler for LLVM.
  llvm::remove_fatal_error_handler();
  llvm::install_fatal_error_handler(llvm_error_handler, NULL);

#if defined(PROVIDE_ARM_CODEGEN)
  LLVMInitializeARMAsmPrinter();
  LLVMInitializeARMTargetMC();
  LLVMInitializeARMTargetInfo();
  LLVMInitializeARMTarget();
  LLVMInitializeARMLDTargetInfo();
  LLVMInitializeARMLDTarget();
  LLVMInitializeARMLDBackend();
  LLVMInitializeARMDiagnosticLineInfo();
#endif

#if defined(PROVIDE_MIPS_CODEGEN)
  LLVMInitializeMipsAsmPrinter();
  LLVMInitializeMipsTargetMC();
  LLVMInitializeMipsTargetInfo();
  LLVMInitializeMipsTarget();
  LLVMInitializeMipsLDTargetInfo();
  LLVMInitializeMipsLDTarget();
  LLVMInitializeMipsLDBackend();
  LLVMInitializeMipsDiagnosticLineInfo();
#endif

#if defined(PROVIDE_X86_CODEGEN)
  LLVMInitializeX86AsmPrinter();
  LLVMInitializeX86TargetMC();
  LLVMInitializeX86TargetInfo();
  LLVMInitializeX86Target();
  LLVMInitializeX86LDTargetInfo();
  LLVMInitializeX86LDTarget();
  LLVMInitializeX86LDBackend();
  LLVMInitializeX86DiagnosticLineInfo();
#endif

  is_initialized = true;

  return;
}

