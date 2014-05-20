//===- TripleOptions.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_TRIPLE_OPTIONS_H
#define MCLD_LDLITE_TRIPLE_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <string>

namespace mcld {

class LinkerConfig;

class TripleOptions
{
public:
  TripleOptions();

  bool parse(int pArgc, char* pArgv[], LinkerConfig& pConfig);

private:
  llvm::cl::opt<std::string>&  m_TargetTriple;
  llvm::cl::opt<std::string>&  m_MArch;
  llvm::cl::opt<std::string>&  m_MCPU;
  llvm::cl::list<std::string>& m_MAttrs;
  llvm::cl::opt<std::string>&  m_Emulation;
};

} // namespace of mcld

#endif

