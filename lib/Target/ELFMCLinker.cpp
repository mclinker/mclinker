//===- ELFMCLinker.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Target/ELFMCLinker.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ELFMCLinker
//===----------------------------------------------------------------------===//
ELFMCLinker::ELFMCLinker(LinkerConfig& pConfig,
                         TargetLDBackend& pLDBackend,
                         mcld::Module& pModule,
                         MemoryArea& pOutput)
  : MCLinker(pConfig, pLDBackend, pModule, pOutput) {
}

ELFMCLinker::~ELFMCLinker()
{
  // MCLinker will delete m_pLDBackend and m_pLDDriver;
}

