//===- MipsELFMCLinker.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "MipsELFMCLinker.h"

using namespace mcld;

MipsELFMCLinker::MipsELFMCLinker(LinkerConfig& pConfig,
                                 mcld::Module& pModule,
                                 FileHandle& pFileHandle)
  : ELFMCLinker(pConfig, pModule, pFileHandle) {
}

MipsELFMCLinker::~MipsELFMCLinker()
{
}

