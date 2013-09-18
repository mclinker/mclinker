//===- OptimizationOptions.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_OPTIMIZATION_OPTIONS_H
#define MCLD_LDLITE_OPTIMIZATION_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <string>

namespace mcld {

class LinkerConfig;

class OptimizationOptions
{
public:
  enum ICF {
    ICF_None,
    ICF_All,
    ICF_Safe
  };

public:
  OptimizationOptions();

  bool parse(LinkerConfig& pConfig);

private:
  bool& m_GCSections;
  llvm::cl::opt<ICF>& m_ICF;
  llvm::cl::list<std::string>& m_Plugin;
  llvm::cl::list<std::string>& m_PluginOpt;
};

} // namespace of mcld

#endif

