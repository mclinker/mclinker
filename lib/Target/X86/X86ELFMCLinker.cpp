//===- X86ELFMCLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86ELFMCLinker.h"

#include <mcld/CodeGen/SectLinkerOption.h>

using namespace mcld;

X86ELFMCLinker::X86ELFMCLinker(SectLinkerOption &pOption,
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

X86ELFMCLinker::~X86ELFMCLinker()
{
}

