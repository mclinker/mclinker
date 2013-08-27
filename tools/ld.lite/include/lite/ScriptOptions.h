//===- ScriptOptions.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_SCRIPT_OPTIONS_H
#define MCLD_LDLITE_SCRIPT_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <string>

namespace mcld {

class LinkerScript;

/** \class ScriptOptions
 *  \brief ScriptOptions are used to modify the default link script. Some
 *  positional options, such as --defsym, also can modify default link script
 *  is not listed here. These special options belong to Positional Options.
 */
class ScriptOptions
{
public:
  ScriptOptions();

  bool parse(LinkerScript& pScript);

private:
  llvm::cl::list<std::string>& m_WrapList;
  llvm::cl::list<std::string>& m_PortList;
  llvm::cl::list<std::string>& m_AddressMapList;
  llvm::cl::opt<unsigned long long>& m_BssSegAddr;
  llvm::cl::opt<unsigned long long>& m_DataSegAddr;
  llvm::cl::opt<unsigned long long>& m_TextSegAddr;
};

} // namespace of mcld

#endif

