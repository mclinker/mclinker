//===- Target.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/Target.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Target
//===----------------------------------------------------------------------===//
Target::Target()
  : TargetMachineCtorFn(NULL),
    MCLinkerCtorFn(NULL),
    TargetLDBackendCtorFn(NULL),
    DiagnosticLineInfoCtorFn(NULL),
    m_pT(NULL) {
}

MCLDTargetMachine* Target::createTargetMachine(const std::string& pTriple,
                                               const std::string& pCPU,
                                               const std::string& pFeatures,
                                               const llvm::TargetOptions& pOptions,
                                               llvm::Reloc::Model pRM,
                                               llvm::CodeModel::Model pCM,
                                               llvm::CodeGenOpt::Level pOL) const
  {
    if (TargetMachineCtorFn && m_pT) {
      llvm::TargetMachine *tm = m_pT->createTargetMachine(pTriple, pCPU, pFeatures, pOptions, pRM, pCM, pOL);
      if (tm)
        return TargetMachineCtorFn(*this, *tm, pTriple);
    }
    return NULL;
  }
