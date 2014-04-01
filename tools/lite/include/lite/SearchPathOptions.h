//===- SearchPathOptions.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_SEARCH_PATH_OPTIONS_H
#define MCLD_LDLITE_SEARCH_PATH_OPTIONS_H
#include <llvm/Support/CommandLine.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/CommandLine.h>
#include <string>

namespace mcld {

class LinkerConfig;
class LinkerScript;

class SearchPathOptions
{
public:
  SearchPathOptions();

  bool parse(LinkerConfig& pConfig, LinkerScript& pScript);

private:
  llvm::cl::opt<mcld::sys::fs::Path,
              false,
              llvm::cl::parser<mcld::sys::fs::Path> >& m_SysRoot;
  llvm::cl::list<std::string,
               bool,
               llvm::cl::SearchDirParser>& m_SearchDirList;
  llvm::cl::opt<bool>& m_NoStdlib;
  llvm::cl::list<std::string,
               bool,
               llvm::cl::SearchDirParser>& m_RuntimePath;

  // not supported yet
  llvm::cl::list<std::string,
               bool,
               llvm::cl::SearchDirParser>& m_RuntimePathLink;
  llvm::cl::list<std::string>& m_Y;
};

} // namespace of mcld

#endif

