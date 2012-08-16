//===- X86ELFMCLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86ELFMCLinker.h"
#include <mcld/Module.h>
#include <mcld/CodeGen/SectLinkerOption.h>

using namespace mcld;

X86ELFMCLinker::X86ELFMCLinker(SectLinkerOption &pOption,
                               TargetLDBackend &pLDBackend,
                               Module& pModule)
  : MCLinker(pOption, pLDBackend, pModule) {
  LinkerConfig &config = pOption.config();
  // set up target-dependent constraints of attibutes
  config.attrFactory().constraint().enableWholeArchive();
  config.attrFactory().constraint().enableAsNeeded();
  config.attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  config.attrFactory().predefined().unsetWholeArchive();
  config.attrFactory().predefined().unsetAsNeeded();
  config.attrFactory().predefined().setDynamic();

}

X86ELFMCLinker::~X86ELFMCLinker()
{
}

