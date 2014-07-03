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
#include <mcld/GeneralOptions.h>
#include <llvm/Support/CommandLine.h>
#include <string>

namespace mcld {

class LinkerConfig;

class OptimizationOptions
{
public:
  OptimizationOptions();

  bool parse(LinkerConfig& pConfig);

private:
  bool& m_GCSections;
  bool& m_PrintGCSections;
  bool& m_GenUnwindInfo;
  llvm::cl::opt<mcld::GeneralOptions::ICF>& m_ICF;
  llvm::cl::opt<unsigned>& m_ICFIterations;
  llvm::cl::opt<bool>& m_PrintICFSections;
  llvm::cl::opt<char>& m_OptLevel;
  llvm::cl::list<std::string>& m_Plugin;
  llvm::cl::list<std::string>& m_PluginOpt;
};

} // namespace of mcld

#endif

