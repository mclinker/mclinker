//===- AndroidSectLinker.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// AndroidSectLinker is a customized linker pass for Android platform.
// This pass set up default parameters for Android.
//
//===----------------------------------------------------------------------===//

#ifndef ANDROID_SECTLINKER_H
#define ANDROID_SECTLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/CodeGen/SectLinker.h>

namespace mcld
{

class AndroidSectLinker : public SectLinker
{
public:
  AndroidSectLinker(const llvm::cl::opt<std::string> &pInputFilename,
                    const std::string &pOutputFilename,
                    unsigned int pOutputLinkType,
                    MCLDInfo& pLDInfo,
                    mcld::TargetLDBackend &pLDBackend);

  virtual ~AndroidSectLinker();

  /// addInputsBeforeCMD - if there are any inputs should add before the
  /// command line, override this function
  virtual void addInputsBeforeCMD(llvm::Module &pM,
                                  MCLDInfo& pLDInfo,
                                  PositionDependentOptions &pOptions);

};

} // namespace of mcld

#endif

