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
  /** create mDriver, mBackend **/
  return kSuccess;
}

enum Linker::ErrorCode Linker::addNameSpec(const std::string &pNameSpec)
{
  return kSuccess;
}

enum Linker::ErrorCode Linker::addObject(const std::string &pObjectPath)
{
  return kSuccess;
}

enum Linker::ErrorCode Linker::addObject(void* pMemory, size_t pSize)
{
  return kSuccess;
}

enum Linker::ErrorCode Linker::addCode(void* pMemory, size_t pSize)
{
  return kSuccess;
}

enum Linker::ErrorCode Linker::setOutput(const std::string &pPath)
{
  return kSuccess;
}

enum Linker::ErrorCode Linker::setOutput(int pFileHandler)
{
  return kSuccess;
}

enum Linker::ErrorCode Linker::link()
{
  return kSuccess;
}

