//===- ARMELFMCLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMELFMCLinker.h"

#include <mcld/LinkerConfig.h>
#include <mcld/Object/SectionMap.h>

using namespace mcld;

ARMELFMCLinker::ARMELFMCLinker(LinkerConfig& pConfig,
                               mcld::Module &pModule,
                               MemoryArea& pOutput,
                               TargetLDBackend &pLDBackend)
  : ELFMCLinker(pConfig, pModule, pOutput, pLDBackend) {
  // set up target-dependent constraints of attributes
  pConfig.attribute().constraint().enableWholeArchive();
  pConfig.attribute().constraint().enableAsNeeded();
  pConfig.attribute().constraint().setSharedSystem();

  // set up the predefined attributes
  pConfig.attribute().predefined().unsetWholeArchive();
  pConfig.attribute().predefined().unsetAsNeeded();
  pConfig.attribute().predefined().setDynamic();

  // set up section map
  pConfig.scripts().sectionMap().push_back(".ARM.exidx", ".ARM.exidx");
  pConfig.scripts().sectionMap().push_back(".ARM.extab", ".ARM.extab");
  pConfig.scripts().sectionMap().push_back(".ARM.attributes", ".ARM.attributes");
}

ARMELFMCLinker::~ARMELFMCLinker()
{
}

