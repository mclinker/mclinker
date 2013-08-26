//===- TargetControlOptions.h ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_TARGET_CONTROL_OPTIONS_H
#define MCLD_LDLITE_TARGET_CONTROL_OPTIONS_H
#include <llvm/Support/CommandLine.h>

namespace mcld {

class LinkerConfig;

class TargetControlOptions
{
public:
  TargetControlOptions();

  bool parse(LinkerConfig& pConfig);

private:
  llvm::cl::opt<int>& m_GPSize;
  llvm::cl::opt<bool>& m_WarnSharedTextrel;
  llvm::cl::opt<bool>& m_FIXCA8;
  llvm::cl::opt<bool>& m_EB;
  llvm::cl::opt<bool>& m_EL;
  llvm::cl::opt<bool>& m_SVR4Compatibility;
};

} // namespace of mcld

#endif

