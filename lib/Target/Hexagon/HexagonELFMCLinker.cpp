//===- HexagonELFMCLinker.cpp ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "HexagonELFMCLinker.h"

using namespace mcld;

HexagonELFMCLinker::HexagonELFMCLinker(LinkerConfig& pConfig,
                                       mcld::Module& pModule,
                                       FileHandle& pFileHandle)
  : ELFMCLinker(pConfig, pModule, pFileHandle) {
}

HexagonELFMCLinker::~HexagonELFMCLinker()
{
}

