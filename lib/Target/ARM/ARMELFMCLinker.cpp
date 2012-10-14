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
  bool exist = false;
  pConfig.scripts().sectionMap().append(".ARM.exidx", ".ARM.exidx", exist);
  pConfig.scripts().sectionMap().append(".ARM.extab", ".ARM.extab", exist);
  pConfig.scripts().sectionMap().append(".ARM.attributes", ".ARM.attributes", exist);
}

ARMELFMCLinker::~ARMELFMCLinker()
{
}

