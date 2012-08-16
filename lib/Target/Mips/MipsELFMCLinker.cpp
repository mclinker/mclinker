//===- MipsELFMCLinker.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "MipsELFMCLinker.h"

#include <mcld/CodeGen/SectLinkerOption.h>

using namespace mcld;

MipsELFMCLinker::MipsELFMCLinker(SectLinkerOption &pOption,
                                 TargetLDBackend &pLDBackend)
  : MCLinker(pOption, pLDBackend) {
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

MipsELFMCLinker::~MipsELFMCLinker()
{
}

