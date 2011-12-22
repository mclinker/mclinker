//===- ARMELFSectLinker.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDFile.h>
#include "ARMELFSectLinker.h"

using namespace mcld;

ARMELFSectLinker::ARMELFSectLinker(const llvm::cl::opt<std::string> &pInputFilename,
                                   const std::string &pOutputFilename,
                                   unsigned int pOutputLinkType,
                                   MCLDInfo& pLDInfo,
                                   TargetLDBackend &pLDBackend)
  : SectLinker(pInputFilename,
               pOutputFilename,
               pOutputLinkType,
               pLDInfo,
               pLDBackend) {
  // set up target-dependent constraints of attibutes
  pLDInfo.attrFactory().constraint().enableWholeArchive();
  pLDInfo.attrFactory().constraint().disableAsNeeded();
  pLDInfo.attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  pLDInfo.attrFactory().predefined().setWholeArchive();
  pLDInfo.attrFactory().predefined().setDynamic();

}

ARMELFSectLinker::~ARMELFSectLinker()
{
}

