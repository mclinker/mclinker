//===- SymbolOptions.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_SYMBOL_OPTIONS_H
#define MCLD_LDLITE_SYMBOL_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <string>

namespace mcld {

class LinkerConfig;

class SymbolOptions
{
public:
  SymbolOptions();

  bool parse(LinkerConfig& pConfig);

private:
  // not supported yet
  llvm::cl::list<std::string>& m_ForceUndefined;
  llvm::cl::opt<std::string>& m_VersionScript;
  llvm::cl::opt<bool>& m_WarnCommon;
  llvm::cl::opt<bool>& m_DefineCommon;
};

} // namespace of mcld

#endif

