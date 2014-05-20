//===- SearchPathOptions.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/SearchPathOptions.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LinkerScript.h>
#include <mcld/Support/raw_ostream.h>

namespace {

llvm::cl::opt<mcld::sys::fs::Path,
              false,
              llvm::cl::parser<mcld::sys::fs::Path> > ArgSysRoot("sysroot",
  llvm::cl::desc("Use directory as the location of the sysroot"),
  llvm::cl::value_desc("directory"),
  llvm::cl::ValueRequired);

llvm::cl::list<std::string,
               bool,
               llvm::cl::SearchDirParser> ArgSearchDirList("L",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Add [searchdir] to the list of search paths"),
  llvm::cl::value_desc("searchdir"),
  llvm::cl::Prefix);

llvm::cl::alias ArgSearchDirListAlias("library-path",
  llvm::cl::desc("alias for -L"),
  llvm::cl::aliasopt(ArgSearchDirList));

llvm::cl::opt<bool> ArgNoStdlib("nostdlib",
  llvm::cl::desc("Only search lib dirs explicitly specified on cmdline"),
  llvm::cl::init(false));

llvm::cl::list<std::string,
               bool,
               llvm::cl::SearchDirParser> ArgRuntimePath("rpath",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Add a directory to the runtime library search path"),
  llvm::cl::value_desc("dir"));

llvm::cl::alias ArgRuntimePathAlias("R",
  llvm::cl::desc("alias for --rpath"),
  llvm::cl::aliasopt(ArgRuntimePath), llvm::cl::Prefix);

// Not supported yet {
llvm::cl::list<std::string,
               bool,
               llvm::cl::SearchDirParser> ArgRuntimePathLink("rpath-link",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Add a directory to the link time library search path"),
  llvm::cl::value_desc("dir"));

llvm::cl::list<std::string> ArgY("Y",
  llvm::cl::desc("Add path to the default library search path"),
  llvm::cl::value_desc("default-search-path"));
// } Not supported yet

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// SearchPathOptions
//===----------------------------------------------------------------------===//
SearchPathOptions::SearchPathOptions()
  : m_SysRoot(ArgSysRoot),
    m_SearchDirList(ArgSearchDirList),
    m_NoStdlib(ArgNoStdlib),
    m_RuntimePath(ArgRuntimePath),
    m_RuntimePathLink(ArgRuntimePathLink),
    m_Y(ArgY) {
}

bool SearchPathOptions::parse(LinkerConfig& pConfig, LinkerScript& pScript)
{
  // set --sysroot
  if (!m_SysRoot.empty()) {
    if (exists(m_SysRoot) && is_directory(m_SysRoot))
      pScript.setSysroot(m_SysRoot);
  }

  // set -L[path]
  llvm::cl::list<std::string>::iterator sd;
  llvm::cl::list<std::string>::iterator sdEnd = m_SearchDirList.end();
  for (sd = m_SearchDirList.begin(); sd != sdEnd; ++sd) {
    if (!pScript.directories().insert(*sd)) {
      // FIXME: need a warning function
      errs() << "WARNING: can not open search directory `-L"
             << *sd
             << "'.\n";
    }
  }

  // set -no-stdlib
  pConfig.options().setNoStdlib(m_NoStdlib);

  // set --rpath [path]
  llvm::cl::list<std::string>::iterator rp;
  llvm::cl::list<std::string>::iterator rpEnd = m_RuntimePath.end();
  for (rp = m_RuntimePath.begin(); rp != rpEnd; ++rp) {
    pConfig.options().getRpathList().push_back(*rp);
  }

  return true;
}

