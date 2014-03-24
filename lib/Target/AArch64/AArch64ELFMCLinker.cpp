//===- AArch64ELFMCLinker.cpp ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "AArch64ELFMCLinker.h"

#include <mcld/LinkerConfig.h>
#include <mcld/Object/SectionMap.h>

using namespace mcld;

AArch64ELFMCLinker::AArch64ELFMCLinker(LinkerConfig& pConfig,
                                       mcld::Module &pModule,
                                       FileHandle& pFileHandle)
  : ELFMCLinker(pConfig, pModule, pFileHandle) {
}

AArch64ELFMCLinker::~AArch64ELFMCLinker()
{
}

