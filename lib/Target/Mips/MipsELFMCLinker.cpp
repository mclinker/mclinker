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
  MCLDInfo &info = pOption.info();
  // set up target-dependent constraints of attibutes
  info.attrFactory().constraint().enableWholeArchive();
  info.attrFactory().constraint().enableAsNeeded();
  info.attrFactory().constraint().setSharedSystem();

  // set up the predefined attributes
  info.attrFactory().predefined().unsetWholeArchive();
  info.attrFactory().predefined().unsetAsNeeded();
  info.attrFactory().predefined().setDynamic();
}

MipsELFMCLinker::~MipsELFMCLinker()
{
}

