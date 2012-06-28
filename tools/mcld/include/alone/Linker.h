//===- Linker.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ALONE_LINKER_H
#define ALONE_LINKER_H

#include <mcld/Support/TargetRegistry.h>
#include <mcld/MC/MCLDDriver.h>

namespace alone {

class LinkerConfig;

class Linker {
public:
  enum ErrorCode {
    kSuccess,

    kMaxErrorCode,
  };

  static const char *GetErrorString(enum ErrorCode pErrCode);

private:
  LinkerConfig *mConfig;
  mcld::TargetLDBackend *mBackend;
  mcld::MCLDDriver *mDriver;

public:
  Linker();

  Linker(const LinkerConfig& pConfig);

  ~Linker();

  enum ErrorCode config(const LinkerConfig& pConfig);
};

} // end namespace alone

#endif // ALONE_LINKER_H
