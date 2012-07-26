//===- LinkerConfig.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "alone/Support/LinkerConfig.h"
#include "alone/Support/Log.h"

#include <llvm/Support/Signals.h>

#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDFile.h>
#include <mcld/MC/MCLDDirectory.h>
#include <mcld/LD/TextDiagnosticPrinter.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/raw_ostream.h>

using namespace alone;

LinkerConfig::LinkerConfig(const std::string &pTriple)
  : mTriple(pTriple), mShared(false), mSOName(), mTarget(NULL), mLDInfo(NULL),
    mDiagLineInfo(NULL), mDiagPrinter(NULL) {

  initializeTarget();
  initializeLDInfo();
  initializeDiagnostic();
}

LinkerConfig::~LinkerConfig() {
  delete mLDInfo;

  if (mDiagPrinter->getNumErrors() != 0) {
    // If here, the program failed ungracefully. Run the interrupt handlers to
    // ensure any other cleanups (e.g., files that registered by
    // RemoveFileOnSignal(...)) getting done before exit.
    llvm::sys::RunInterruptHandlers();
  }
  mDiagPrinter->finish();

  delete mDiagLineInfo;
  delete mDiagPrinter;
}

bool LinkerConfig::initializeTarget() {
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

bool LinkerConfig::initializeLDInfo() {
  if (NULL != mLDInfo) {
    ALOGE("Cannot initialize mcld::MCLDInfo for given triple '%s!\n",
          mTriple.c_str());
    return false;
  }

  mLDInfo = new mcld::MCLDInfo(getTriple(), 1, 32);
  return true;
}

bool LinkerConfig::initializeDiagnostic() {
  // Set up MsgHandler.
  mDiagLineInfo = mTarget->createDiagnosticLineInfo(*mTarget, mTriple);

  mDiagPrinter = new mcld::TextDiagnosticPrinter(mcld::errs(), *mLDInfo);

  mcld::InitializeDiagnosticEngine(*mLDInfo, mDiagLineInfo, mDiagPrinter);

  return true;
}

void LinkerConfig::setShared(bool pEnable) {
  mShared = pEnable;
  return;
}

void LinkerConfig::setBsymbolic(bool pEnable) {
  mLDInfo->options().setBsymbolic(pEnable);
  return;
}

void LinkerConfig::setSOName(const std::string &pSOName) {
  mSOName = pSOName;
  return;
}

void LinkerConfig::setDyld(const std::string &pDyld) {
  mLDInfo->options().setDyld(pDyld);
  return;
}

void LinkerConfig::setSysRoot(const std::string &pSysRoot) {
  mLDInfo->options().setSysroot(mcld::sys::fs::Path(pSysRoot));
  return;
}

void LinkerConfig::addWrap(const std::string &pWrapSymbol) {
  bool exist = false;

  // Add wname -> __wrap_wname.
  mcld::StringEntry<llvm::StringRef>* to_wrap =
               mLDInfo->scripts().renameMap().insert(pWrapSymbol, exist);

  std::string to_wrap_str = "__wrap_" + pWrapSymbol;
  to_wrap->setValue(to_wrap_str);

  if (exist) {
    mcld::warning(mcld::diag::rewrap) << pWrapSymbol << to_wrap_str;
  }

  // Add __real_wname -> wname.
  std::string from_real_str = "__real_" + pWrapSymbol;
  mcld::StringEntry<llvm::StringRef>* from_real =
             mLDInfo->scripts().renameMap().insert(from_real_str, exist);
  from_real->setValue(pWrapSymbol);

  if (exist) {
    mcld::warning(mcld::diag::rewrap) << pWrapSymbol << from_real_str;
  }

  return;
}

void LinkerConfig::addPortable(const std::string &pPortableSymbol) {
  bool exist = false;

  // Add pname -> pname_portable.
  mcld::StringEntry<llvm::StringRef>* to_port =
                mLDInfo->scripts().renameMap().insert(pPortableSymbol, exist);

  std::string to_port_str = pPortableSymbol + "_portable";
  to_port->setValue(to_port_str);

  if (exist) {
    mcld::warning(mcld::diag::rewrap) << pPortableSymbol << to_port_str;
}

  // Add __real_pname -> pname.
  std::string from_real_str = "__real_" + pPortableSymbol;
  mcld::StringEntry<llvm::StringRef>* from_real =
           mLDInfo->scripts().renameMap().insert(from_real_str, exist);

  from_real->setValue(pPortableSymbol);

  if (exist) {
    mcld::warning(mcld::diag::rewrap) << pPortableSymbol << from_real_str;
  }

  return;
}

void LinkerConfig::addSearchDir(const std::string &pDirPath) {
  // SearchDirs will remove the created MCLDDirectory.
  mcld::MCLDDirectory* sd = new mcld::MCLDDirectory(pDirPath);

  if (sd->isInSysroot()) {
    sd->setSysroot(mLDInfo->options().sysroot());
  }

  if (exists(sd->path()) && is_directory(sd->path())) {
    mLDInfo->options().directories().add(*sd);
  } else {
    mcld::warning(mcld::diag::warn_cannot_open_search_dir) << sd->name();
  }

  return;
}
