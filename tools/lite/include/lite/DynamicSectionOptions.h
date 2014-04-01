//===- DynamicSectionOptions.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_DYNAMIC_SECTION_OPTIONS_H
#define MCLD_LDLITE_DYNAMIC_SECTION_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <mcld/Support/CommandLine.h>
#include <string>

namespace mcld {

class ZOption;
class LinkerConfig;
class LinkerScript;

class DynamicSectionOptions
{
public:
  DynamicSectionOptions();

  bool parse(LinkerConfig& pConfig, LinkerScript& pScript);

private:
  llvm::cl::opt<std::string>& m_Entry;
  llvm::cl::opt<bool>& m_Bsymbolic;
  llvm::cl::opt<bool>& m_Bgroup;
  llvm::cl::opt<std::string>& m_SOName;
  llvm::cl::opt<llvm::cl::boolOrDefault>& m_NoUndefined;
  llvm::cl::opt<llvm::cl::boolOrDefault>& m_AllowMulDefs;
  llvm::cl::list<ZOption,
                 bool,
                 llvm::cl::parser<ZOption> >& m_ZOptionList;
  llvm::cl::opt<std::string>& m_Dyld;
  llvm::cl::opt<bool>& m_EnableNewDTags;

  llvm::cl::list<std::string>& m_Auxiliary;
  llvm::cl::opt<std::string>& m_Filter;
};

} // namespace of mcld

#endif

