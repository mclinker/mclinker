//===- ARMELFMCLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMELFMCLinker.h"

#include <mcld/Module.h>
#include <mcld/MC/InputFactory.h>
#include <mcld/CodeGen/SectLinkerOption.h>

using namespace mcld;

ARMELFMCLinker::ARMELFMCLinker(SectLinkerOption &pOption,
                               TargetLDBackend &pLDBackend,
                               mcld::Module &pModule,
                               MemoryArea& pOutput)
  : MCLinker(pOption, pLDBackend, pModule, pOutput) {
  LinkerConfig &config = pOption.config();
  // set up target-dependent constraints of attributes
  config.inputFactory().constraint().enableWholeArchive();
  config.inputFactory().constraint().enableAsNeeded();
  config.inputFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  config.inputFactory().predefined().unsetWholeArchive();
  config.inputFactory().predefined().unsetAsNeeded();
  config.inputFactory().predefined().setDynamic();

}

ARMELFMCLinker::~ARMELFMCLinker()
{
}

