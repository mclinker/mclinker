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
#include "alone/Support/Log.h"

#include <llvm/Support/ELF.h>

#include <mcld/Module.h>
#include <mcld/IRBuilder.h>
#include <mcld/InputTree.h>
#include <mcld/Object/ObjectLinker.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/MC/InputFactory.h>
#include <mcld/MC/ContextFactory.h>
#include <mcld/MC/InputBuilder.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDContext.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MemoryAreaFactory.h>
#include <mcld/Support/TargetRegistry.h>

using namespace alone;

const char* Linker::GetErrorString(enum Linker::ErrorCode pErrCode) {
  static const char* ErrorString[] = {
    /* kSuccess */
    "Successfully compiled.",
    /* kDoubleConfig */
    "Configure Linker twice.",
    /* kCreateBackend */
    "Cannot create backend.",
    /* kDelegateLDInfo */
    "Cannot get linker information",
    /* kFindNameSpec */
    "Cannot find -lnamespec",
    /* kOpenNameSpec */
    "Cannot open -lnamespec",
    /* kOpenObjectFile */
    "Cannot open object file",
    /* kNotConfig */
    "Linker::config() is not called",
    /* kNotSetUpOutput */
    "Linker::setOutput() is not called before add input files",
    /* kOpenOutput */
    "Cannot open output file",
    /* kReadSections */
    "Cannot read sections",
    /* kReadSymbols */
    "Cannot read symbols",
    /* kAddAdditionalSymbols */
    "Cannot add standard and target symbols",
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
  : mLDConfig(NULL), mModule(NULL), mBackend(NULL), mObjLinker(NULL),
    mInputFactory(NULL), mMemAreaFactory(NULL), mContextFactory(NULL),
    mBuilder(NULL), mRoot(NULL), mOutput(NULL) {
}

Linker::Linker(const LinkerConfig& pConfig)
  : mLDConfig(NULL), mModule(NULL), mBackend(NULL), mObjLinker(NULL),
    mInputFactory(NULL), mMemAreaFactory(NULL), mContextFactory(NULL),
    mBuilder(NULL), mRoot(NULL), mOutput(NULL) {

  const std::string &triple = pConfig.getTriple();

  enum ErrorCode err = config(pConfig);
  if (kSuccess != err) {
    ALOGE("%s (%s)", GetErrorString(err), triple.c_str());
    return;
  }

  return;
}

Linker::~Linker() {
  delete mModule;
  delete mObjLinker;
  // FIXME: current implementation can not change the order of deleting
  // ObjectLinker and TargetLDBackend. Because the deletion of relocation list
  // in FragmentLinker (FragmentLinker is deleted by ObjectLinker) depends on
  // RelocationFactory in TargetLDBackend
  delete mBackend;
  delete mBuilder;
  delete mRoot;
}

enum Linker::ErrorCode Linker::extractFiles(const LinkerConfig& pConfig) {
  mLDConfig = pConfig.getLDConfig();
  if (mLDConfig == NULL) {
    return kDelegateLDInfo;
  }
  return kSuccess;
}

enum Linker::ErrorCode Linker::config(const LinkerConfig& pConfig) {
  if (mLDConfig != NULL) {
    return kDoubleConfig;
  }

  extractFiles(pConfig);

  mBackend = pConfig.getTarget()->createLDBackend(*mLDConfig);
  if (mBackend == NULL) {
    return kCreateBackend;
  }

  mInputFactory = new mcld::InputFactory(32, *mLDConfig);

  mContextFactory = new mcld::ContextFactory(32);
    /* 32 is a magic number, the estimated number of input files **/

  mMemAreaFactory = new mcld::MemoryAreaFactory(32);

  mBuilder = new mcld::InputBuilder(*mLDConfig,
                                    *mInputFactory,
                                    *mContextFactory,
                                    *mMemAreaFactory,
                                    true); // delegated

  mModule = new mcld::Module(mLDConfig->options().soname());

  mRoot = new mcld::InputTree::iterator(mModule->getInputTree().root());

  mObjLinker = new mcld::ObjectLinker(*mLDConfig, *mModule, *mBuilder, *mBackend);

  mObjLinker->initFragmentLinker();

  return kSuccess;
}

void Linker::advanceRoot() {
  if (mRoot->isRoot()) {
    mRoot->move<mcld::TreeIteratorBase::Leftward>();
  } else {
    mRoot->move<mcld::TreeIteratorBase::Rightward>();
  }
  return;
}

enum Linker::ErrorCode Linker::openFile(const mcld::sys::fs::Path& pPath,
                                        enum Linker::ErrorCode pCode,
                                        mcld::Input& pInput) {
  mcld::MemoryArea *input_memory = mMemAreaFactory->produce(pPath,
                                                    mcld::FileHandle::ReadOnly);

  if (input_memory->handler()->isGood()) {
    pInput.setMemArea(input_memory);
  } else {
    return pCode;
  }

  mBuilder->setContext(pInput);
  return kSuccess;
}

enum Linker::ErrorCode Linker::addNameSpec(const std::string &pNameSpec) {
  const mcld::sys::fs::Path* path = NULL;
  // find out the real path of the namespec.
  if (mLDConfig->attribute().constraint().isSharedSystem()) {
    // In the system with shared object support, we can find both archive
    // and shared object.

    if (mInputFactory->attr().isStatic()) {
      // with --static, we must search an archive.
      path = mLDConfig->options().directories().find(pNameSpec,
                                                     mcld::Input::Archive);
    }
    else {
      // otherwise, with --Bdynamic, we can find either an archive or a
      // shared object.
      path = mLDConfig->options().directories().find(pNameSpec,
                                                     mcld::Input::DynObj);
    }
  }
  else {
    // In the system without shared object support, we only look for an
    // archive.
    path = mLDConfig->options().directories().find(pNameSpec,
                                                 mcld::Input::Archive);
  }

  if (NULL == path)
    return kFindNameSpec;

  mcld::Input* input = mInputFactory->produce(pNameSpec, *path,
                                              mcld::Input::Unknown);
  mModule->getInputTree().insert<mcld::InputTree::Positional>(*mRoot, *input);

  advanceRoot();

  return openFile(*path, kOpenNameSpec, *input);
}

/// addObject - Add a object file by the filename.
enum Linker::ErrorCode Linker::addObject(const std::string &pObjectPath) {
  mcld::Input* input = mInputFactory->produce(pObjectPath,
                                              pObjectPath,
                                              mcld::Input::Unknown);

  mModule->getInputTree().insert<mcld::InputTree::Positional>(*mRoot, *input);

  advanceRoot();

  return openFile(pObjectPath, kOpenObjectFile, *input);
}

/// addObject - Add a piece of memory. The memory is of ELF format.
enum Linker::ErrorCode Linker::addObject(void* pMemory, size_t pSize) {

  mcld::Input* input = mInputFactory->produce("memory object", "NAN",
                                              mcld::Input::Unknown);

  mModule->getInputTree().insert<mcld::InputTree::Positional>(*mRoot, *input);

  advanceRoot();

  mcld::MemoryArea *input_memory = mMemAreaFactory->produce(pMemory, pSize);
  input->setMemArea(input_memory);

  mcld::LDContext *input_context = mContextFactory->produce();
  input->setContext(input_context);

  return kSuccess;
}

enum Linker::ErrorCode Linker::addCode(void* pMemory, size_t pSize) {
  mcld::Input* input = mInputFactory->produce("code object", "NAN",
                                              mcld::Input::External);

  mModule->getInputTree().insert<mcld::InputTree::Positional>(*mRoot, *input);

  advanceRoot();

  mcld::MemoryArea *input_memory = mMemAreaFactory->produce(pMemory, pSize);
  input->setMemArea(input_memory);

  mcld::LDContext *input_context = mContextFactory->produce();
  input->setContext(input_context);

  // FIXME: So far, FragmentLinker must set up output before add input files.
  // set up LDContext
  if (mObjLinker->hasInitLinker()) {
    return kNotConfig;
  }

  // create NULL section
  mcld::LDSection* null = mcld::IRBuilder::CreateELFHeader(*input, "",
                              llvm::ELF::SHT_NULL, 0, 0);
  null->setSize(0);
  null->setOffset(0);
  null->setInfo(0);

  // create .text section
  mcld::LDSection* text = mcld::IRBuilder::CreateELFHeader(*input, ".text",
                              llvm::ELF::SHT_PROGBITS,
                              llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_EXECINSTR,
                              1);

  text->setSize(pSize);
  text->setOffset(0x0);
  text->setInfo(0);

  return kSuccess;
}

enum Linker::ErrorCode Linker::setOutput(const std::string &pPath) {
  // -----  initialize output file  ----- //
  mcld::FileHandle::Permission perm = 0755;

  mOutput = mMemAreaFactory->produce(
                      pPath,
                      mcld::FileHandle::ReadWrite |
                        mcld::FileHandle::Truncate |
                        mcld::FileHandle::Create,
                      perm);

  if (!mOutput->handler()->isGood()) {
    return kOpenOutput;
  }

  return kSuccess;
}

enum Linker::ErrorCode Linker::setOutput(int pFileHandler) {
  mOutput = mMemAreaFactory->produce(pFileHandler, mcld::FileHandle::ReadWrite);

  if (!mOutput->handler()->isGood()) {
    return kOpenOutput;
  }

  return kSuccess;
}

enum Linker::ErrorCode Linker::link() {
  if (NULL == mOutput)
    return kNotSetUpOutput;

  if (!mObjLinker->hasInitLinker()) {
    return kNotConfig;
  }

  mObjLinker->initStdSections();

  mObjLinker->normalize();

  if (!mObjLinker->readRelocations())
    return kReadSections;

  if (!mObjLinker->mergeSections())
    return kReadSections;

  if (!mObjLinker->addStandardSymbols() || !mObjLinker->addTargetSymbols()) {
    return kAddAdditionalSymbols;
  }

  mObjLinker->scanRelocations();
  mObjLinker->prelayout();
  mObjLinker->layout();
  mObjLinker->postlayout();
  mObjLinker->finalizeSymbolValue();
  mObjLinker->relocation();
  mObjLinker->emitOutput(*mOutput);
  mObjLinker->postProcessing(*mOutput);

  return kSuccess;
}

