//===- ARMELFSectLinker.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMELFSectLinker.h"

#include <mcld/CodeGen/SectLinkerOption.h>

using namespace mcld;

ARMELFSectLinker::ARMELFSectLinker(SectLinkerOption &pOption,
                                   TargetLDBackend &pLDBackend)
  : SectLinker(pOption,
               pLDBackend) {
  MCLDInfo &info = pOption.info();
  // set up target-dependent constraints of attributes
  info.attrFactory().constraint().enableWholeArchive();
  info.attrFactory().constraint().disableAsNeeded();
  info.attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  info.attrFactory().predefined().setWholeArchive();
  info.attrFactory().predefined().setDynamic();

}

ARMELFSectLinker::~ARMELFSectLinker()
{
}

