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

#include <string>

namespace mcld {

class Module;
class TargetLDBackend;
class ObjectLinker;
class ContextFactory;
class LinkerConfig;
class TreeIteratorBase;
class Input;
class InputFactory;
class InputBuilder;
class MemoryArea;
class MemoryAreaFactory;

namespace sys { namespace fs {

class Path;

} } // end namespace sys::fs

} // end namespace mcld

namespace alone {

class LinkerConfig;

class Linker {
public:
  enum ErrorCode {
    kSuccess,
    kDoubleConfig,
    kCreateBackend,
    kDelegateLDInfo,
    kFindNameSpec,
    kOpenNameSpec,
    kOpenObjectFile,
    kNotConfig,
    kNotSetUpOutput,
    kOpenOutput,
    kReadSections,
    kReadSymbols,
    kAddAdditionalSymbols,
    kMaxErrorCode
  };

  static const char *GetErrorString(enum ErrorCode pErrCode);

private:
  const mcld::LinkerConfig *mLDConfig;
  mcld::Module *mModule;
  mcld::TargetLDBackend *mBackend;
  mcld::ObjectLinker *mObjLinker;
  mcld::InputFactory *mInputFactory;
  mcld::MemoryAreaFactory *mMemAreaFactory;
  mcld::ContextFactory *mContextFactory;
  mcld::InputBuilder *mBuilder;
  mcld::TreeIteratorBase *mRoot;
  std::string mSOName;
  mcld::MemoryArea* mOutput;

public:
  Linker();

  Linker(const LinkerConfig& pConfig);

  ~Linker();

  enum ErrorCode config(const LinkerConfig& pConfig);

  enum ErrorCode addNameSpec(const std::string &pNameSpec);

  enum ErrorCode addObject(const std::string &pObjectPath);

  enum ErrorCode addObject(void* pMemory, size_t pSize);

  enum ErrorCode addCode(void* pMemory, size_t pSize);

  enum ErrorCode setOutput(const std::string &pPath);

  enum ErrorCode setOutput(int pFileHandler);

  enum ErrorCode link();

private:
  enum ErrorCode extractFiles(const LinkerConfig& pConfig);

  enum ErrorCode openFile(const mcld::sys::fs::Path& pPath,
                          enum ErrorCode pCode,
                          mcld::Input& pInput);

  void advanceRoot();
};

} // end namespace alone

#endif // ALONE_LINKER_H
