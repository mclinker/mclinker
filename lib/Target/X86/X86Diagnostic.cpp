//===- X86Diagnostic.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/ADT/Triple.h>
#include <mcld/Support/TargetRegistry.h>
#include "X86.h"
#include "X86Diagnostic.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// X86Diagnostic


namespace mcld {
//===----------------------------------------------------------------------===//
// createX86Diagnostic - the help function to create corresponding X86Diagnostic
//
LDDiagnostic* createX86Diagnostic(const llvm::Target& pTarget,
                                  const std::string &pTriple)
{
  return new X86Diagnostic();
}

} // namespace of mcld

//==========================
// InitializeX86Diagnostic
extern "C" void LLVMInitializeX86Diagnostic() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterDiagnostic(TheX86Target, createX86Diagnostic);
}

