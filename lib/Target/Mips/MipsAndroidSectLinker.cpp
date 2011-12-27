//===- MipsAndroidSectLinker.cpp ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Target/AndroidSectLinker.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDFile.h>
#include "MipsAndroidSectLinker.h"

using namespace mcld;

MipsAndroidSectLinker::MipsAndroidSectLinker(const llvm::cl::opt<std::string> &pInputFilename,
                                             const std::string &pOutputFilename,
                                             unsigned int pOutputLinkType,
                                             MCLDInfo& pLDInfo,
                                             TargetLDBackend &pLDBackend)
  : AndroidSectLinker(pInputFilename,
                      pOutputFilename,
                      pOutputLinkType,
                      pLDInfo,
                      pLDBackend) {
  // set up target-dependent constraints of attibutes
  pLDInfo.attrFactory().constraint().disableWholeArchive();
  pLDInfo.attrFactory().constraint().disableAsNeeded();
  pLDInfo.attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  pLDInfo.attrFactory().predefined().unsetWholeArchive();
  pLDInfo.attrFactory().predefined().setDynamic();
}

MipsAndroidSectLinker::~MipsAndroidSectLinker()
{
}
