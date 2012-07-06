//===- LinkerConfig.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "alone/Support/LinkerConfig.h"

#include <llvm/Support/Signals.h>

#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDDirectory.h>
#include <mcld/LD/TextDiagnosticPrinter.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/raw_ostream.h>

using namespace alone;

LinkerConfig::LinkerConfig(const std::string &pTriple)
  : mTriple(pTriple), mTarget(NULL), mLDInfo(NULL), mDiagLineInfo(NULL),
    mDiagPrinter(NULL) {

  initializeTarget();
  initializeLDInfo();
  initializeDiagnostic();
}

LinkerConfig::~LinkerConfig()
{
  if (NULL != mLDInfo)
    delete mLDInfo;

  if (0 != mDiagPrinter->getNumErrors()) {
    // If we reached here, we are failing ungracefully. Run the interrupt handlers
    // to make sure any special cleanups get done, in particular that we remove
    // files registered with RemoveFileOnSignal.
    llvm::sys::RunInterruptHandlers();
  }
  mDiagPrinter->finish();

  delete mDiagLineInfo;
  delete mDiagPrinter;
}

bool LinkerConfig::initializeTarget()
{
  std::string error;
  mTarget = mcld::TargetRegistry::lookupTarget(mTriple, error);
  if (NULL != mTarget) {
    return true;
  } else {
    ALOGE("Cannot initialize mcld::Target for given triple '%s'! (%s)\n",
          mTriple.c_str(), error.c_str());
    return false;
  }
}

bool LinkerConfig::initializeLDInfo()
{
  if (NULL != mLDInfo) {
    ALOGE("Cannot initialize mcld::MCLDInfo for given triple '%s!\n",
          mTriple.c_str());
    return false;
  }

  mLDInfo = new mcld::MCLDInfo(getTriple(), 1, 32);
  return true;
}

bool LinkerConfig::initializeDiagnostic()
{
  // Set up MsgHandler
  mDiagLineInfo = mTarget->createDiagnosticLineInfo(*mTarget->get(), mTriple);

  mDiagPrinter = new mcld::TextDiagnosticPrinter(mcld::errs(), *mLDInfo);

  mcld::InitializeDiagnosticEngine(*mLDInfo, mDiagLineInfo, mDiagPrinter);

  return true;
}

void LinkerConfig::setSysRoot(const std::string &pSysRoot)
{
  mLDInfo->options().setSysroot(mcld::sys::fs::Path(pSysRoot));
}

void LinkerConfig::addWrap(const std::string &pWrapSymbol)
{
  bool exist = false;

  // add wname -> __wrap_wname
  mcld::StringEntry<llvm::StringRef>* to_wrap =
               mLDInfo->scripts().renameMap().insert(pWrapSymbol, exist);

  std::string to_wrap_str = "__wrap_" + pWrapSymbol;
  to_wrap->setValue(to_wrap_str);

  if (exist)
    mcld::warning(mcld::diag::rewrap) << pWrapSymbol << to_wrap_str;

  // add __real_wname -> wname
  std::string from_real_str = "__real_" + pWrapSymbol;
  mcld::StringEntry<llvm::StringRef>* from_real =
             mLDInfo->scripts().renameMap().insert(from_real_str, exist);
  from_real->setValue(pWrapSymbol);

  if (exist)
    mcld::warning(mcld::diag::rewrap) << pWrapSymbol << from_real_str;
}

void LinkerConfig::addSearchDir(const std::string &pDirPath)
{
  // SearchDirs will remove the created MCLDDirectory.
  mcld::MCLDDirectory* sd = new mcld::MCLDDirectory(pDirPath);

  if (sd->isInSysroot())
    sd->setSysroot(mLDInfo->options().sysroot());
  if (exists(sd->path()) && is_directory(sd->path())) {
    mLDInfo->options().directories().add(*sd);
  }
  else
    mcld::warning(mcld::diag::warn_cannot_open_search_dir) << sd->name();
}

