//===- PreferenceOptions.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_PREFERENCE_OPTIONS_H
#define MCLD_LDLITE_PREFERENCE_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <string>

namespace mcld {

class LinkerConfig;

class PreferenceOptions
{
public:
  enum Color {
    COLOR_Never,
    COLOR_Always,
    COLOR_Auto
  };

public:
  PreferenceOptions();

  bool parse(LinkerConfig& pConfig);

private:
  llvm::cl::opt<bool>&  m_Trace;
  llvm::cl::opt<int>&   m_Verbose;
  llvm::cl::opt<bool>&  m_Version;
  llvm::cl::opt<int>&   m_MaxErrorNum;
  llvm::cl::opt<int>&   m_MaxWarnNum;
  llvm::cl::opt<Color>& m_Color;
  llvm::cl::opt<bool>&  m_PrintMap;
  bool& m_FatalWarnings;
};

} // namespace of mcld

#endif

