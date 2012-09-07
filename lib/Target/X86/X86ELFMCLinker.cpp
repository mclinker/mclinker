//===- X86ELFMCLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86ELFMCLinker.h"
#include <mcld/LinkerConfig.h>

using namespace mcld;

X86ELFMCLinker::X86ELFMCLinker(LinkerConfig& pConfig,
                               TargetLDBackend &pLDBackend,
                               Module& pModule,
                               MemoryArea& pOutput)
  : MCLinker(pConfig, pLDBackend, pModule, pOutput) {
  // set up target-dependent constraints of attibutes
  pConfig.attribute().constraint().enableWholeArchive();
  pConfig.attribute().constraint().enableAsNeeded();
  pConfig.attribute().constraint().setSharedSystem();

  // set up the predefined attributes
  pConfig.attribute().predefined().unsetWholeArchive();
  pConfig.attribute().predefined().unsetAsNeeded();
  pConfig.attribute().predefined().setDynamic();

}

X86ELFMCLinker::~X86ELFMCLinker()
{
}

