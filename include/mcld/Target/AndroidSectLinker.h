//===- AndroidSectLinker.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ANDROIDSECTLINKER_H
#define ANDROIDSECTLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/CodeGen/SectLinker.h>

namespace mcld
{

/** \class AndroidSectLinker
 *  \brief Section Linker for Android (ELF format is used).
 *
 *  \see
 *  \author Anders Cheng <Anders.Cheng@mediatek.com>
 */

class AndroidSectLinker : public SectLinker
{
public:
  AndroidSectLinker(const std::string &pInputFilename,
                   const std::string &pOutputFilename,
                   unsigned int pOutputLinkType,
                   MCLDInfo& pLDInfo,
                   mcld::TargetLDBackend &pLDBackend);

  virtual ~AndroidSectLinker();

  virtual bool doInitialization(llvm::Module &pM);
};

} // namespace of mcld

#endif

