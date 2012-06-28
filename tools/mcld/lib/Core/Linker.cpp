//===- Linker.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "alone/Linker.h"
#include "alone/Support/LinkerConfig.h"

using namespace alone;

const char* Linker::GetErrorString(enum Linker::ErrorCode pErrCode)
{
  static const char* ErrorString[] = {
    /* kSuccess */
    "Successfully compiled.",
    /* kMaxErrorCode */
    "(Unknown error code)"
  };

  if (pErrCode > kMaxErrorCode) {
    pErrCode = kMaxErrorCode;
  }

  return ErrorString[ static_cast<size_t>(pErrCode) ];
}

//===----------------------------------------------------------------------===//
// Linker
//===----------------------------------------------------------------------===//
Linker::Linker()
  : mConfig(NULL), mBackend(NULL), mDriver(NULL) {
}

Linker::Linker(const LinkerConfig& pConfig)
  : mConfig(NULL), mBackend(NULL), mDriver(NULL) {

  const std::string &triple = pConfig.getTriple();

  enum ErrorCode err = config(pConfig);
  if (kSuccess != err) {
    ALOGE("%s (%s)", GetErrorString(err), triple.c_str());
    return;
  }

  return;
}

Linker::~Linker()
{
}

enum Linker::ErrorCode Linker::config(const LinkerConfig& pConfig)
{
  /** **/
  return kSuccess;
}

