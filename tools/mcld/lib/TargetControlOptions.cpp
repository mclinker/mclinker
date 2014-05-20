//===- TargetControlOptions.cpp -------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/TargetControlOptions.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Support/MsgHandling.h>

namespace {

llvm::cl::opt<int> ArgGPSize("G",
  llvm::cl::desc("Set the maximum size of objects to be optimized using GP"),
  llvm::cl::init(8));

llvm::cl::opt<bool> ArgWarnSharedTextrel("warn-shared-textrel",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Warn if adding DT_TEXTREL in a shared object."),
  llvm::cl::init(false));

// Not supported yet {
llvm::cl::opt<bool> ArgFIXCA8("fix-cortex-a8",
  llvm::cl::desc("Enable Cortex-A8 Thumb-2 branch erratum fix"),
  llvm::cl::init(false));

llvm::cl::opt<bool> ArgEB("EB",
  llvm::cl::desc("Link big-endian objects. This affects the output format."),
  llvm::cl::init(false));

llvm::cl::opt<bool> ArgEL("EL",
  llvm::cl::desc("Link little-endian objects. This affects the output format."),
  llvm::cl::init(false));

llvm::cl::opt<bool> ArgSVR4Compatibility("Qy",
  llvm::cl::desc("This option is ignored for SVR4 compatibility"),
  llvm::cl::init(false));

// } Not supported yet

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// TargetControlOptions
//===----------------------------------------------------------------------===//
TargetControlOptions::TargetControlOptions()
  : m_GPSize(ArgGPSize),
    m_WarnSharedTextrel(ArgWarnSharedTextrel),
    m_FIXCA8(ArgFIXCA8),
    m_EB(ArgEB),
    m_EL(ArgEL),
    m_SVR4Compatibility(ArgSVR4Compatibility) {
}

bool TargetControlOptions::parse(LinkerConfig& pConfig)
{
  // set -G [size]
  pConfig.options().setGPSize(m_GPSize);

  // set --warn-shared-textrel
  pConfig.options().setWarnSharedTextrel(m_WarnSharedTextrel);

  // set --fix-cortex-a8
  if (m_FIXCA8)
    mcld::warning(mcld::diag::warn_unsupported_option) << m_FIXCA8.ArgStr;

  return true;
}

