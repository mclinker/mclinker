//===- SymbolOptions.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <lite/SymbolOptions.h>
#include <mcld/LinkerConfig.h>

namespace {

llvm::cl::opt<bool> ArgNoUndefined("no-undefined",
  llvm::cl::desc("Do not allow unresolved references"),
  llvm::cl::init(false));

llvm::cl::opt<bool> ArgAllowMulDefs("allow-multiple-definition",
  llvm::cl::desc("Allow multiple definition"),
  llvm::cl::init(false));

// Not supprted yet {
llvm::cl::opt<std::string> ArgForceUndefined("u",
  llvm::cl::desc("Force symbol to be undefined in the output file"),
  llvm::cl::value_desc("symbol"));

llvm::cl::alias ArgForceUndefinedAlias("undefined",
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
  : m_NoUndefined(ArgNoUndefined),
    m_AllowMulDefs(ArgAllowMulDefs),
    m_ForceUndefined(ArgForceUndefined),
    m_VersionScript(ArgVersionScript),
    m_WarnCommon(ArgWarnCommon),
    m_DefineCommon(ArgDefineCommon) {
}

bool SymbolOptions::parse(LinkerConfig& pConfig)
{
  // set --no-undefined
  pConfig.options().setNoUndefined(m_NoUndefined);

  // set --allow-multiple-definition
  pConfig.options().setMulDefs(m_AllowMulDefs);

  // set -d
  pConfig.options().setDefineCommon(m_DefineCommon);

  return true;
}

