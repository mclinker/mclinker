//===- ARMELFMCLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ARMELFMCLinker.h"

using namespace mcld;

ARMELFMCLinker::ARMELFMCLinker(LinkerConfig& pConfig,
                               mcld::Module &pModule,
                               FileHandle& pFileHandle)
  : ELFMCLinker(pConfig, pModule, pFileHandle) {
}

ARMELFMCLinker::~ARMELFMCLinker()
{
}

