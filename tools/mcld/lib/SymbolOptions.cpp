//===- SymbolOptions.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/SymbolOptions.h>
#include <mcld/LinkerConfig.h>

namespace {

// Not supprted yet {
llvm::cl::list<std::string> ArgForceUndefined("u",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Force symbol to be undefined in the output file"),
  llvm::cl::value_desc("symbol"));

llvm::cl::alias ArgForceUndefinedAlias("undefined",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("alias for -u"),
  llvm::cl::aliasopt(ArgForceUndefined));

llvm::cl::opt<std::string> ArgVersionScript("version-script",
  llvm::cl::desc("Version script."),
  llvm::cl::value_desc("Version script"));

llvm::cl::opt<bool> ArgWarnCommon("warn-common",
  llvm::cl::desc("warn common symbol"),
  llvm::cl::init(false));

llvm::cl::opt<bool> ArgDefineCommon("d",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Define common symbol"),
  llvm::cl::init(false));

llvm::cl::alias ArgDefineCommonAlias1("dc",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("alias for -d"),
  llvm::cl::aliasopt(ArgDefineCommon));

llvm::cl::alias ArgDefineCommonAlias2("dp",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("alias for -d"),
  llvm::cl::aliasopt(ArgDefineCommon));

// } Not supported yet

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// SymbolOptions
//===----------------------------------------------------------------------===//
SymbolOptions::SymbolOptions()
  : m_ForceUndefined(ArgForceUndefined),
    m_VersionScript(ArgVersionScript),
    m_WarnCommon(ArgWarnCommon),
    m_DefineCommon(ArgDefineCommon) {
}

bool SymbolOptions::parse(LinkerConfig& pConfig)
{
  // set -d
  pConfig.options().setDefineCommon(m_DefineCommon);

  // set -u/--undefined symbols
  llvm::cl::list<std::string>::iterator usym, usymEnd = m_ForceUndefined.end();
  for (usym = m_ForceUndefined.begin(); usym != usymEnd; ++usym)
    pConfig.options().getUndefSymList().push_back(*usym);

  return true;
}

