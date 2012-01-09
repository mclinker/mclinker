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
  AndroidSectLinker(SectLinkerOption &pOption,
                    mcld::TargetLDBackend &pLDBackend);

  virtual ~AndroidSectLinker();

  // addTargetInputs - add Android-specific linker options
  virtual void addTargetOptions(llvm::Module &pM,
                                SectLinkerOption &pOption);

};

} // namespace of mcld

#endif

