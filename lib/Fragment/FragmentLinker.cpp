//===- FragmentLinker.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the FragmentLinker class
//
//===----------------------------------------------------------------------===//
#include <mcld/Fragment/FragmentLinker.h>

#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>

#include <mcld/LinkerConfig.h>
#include <mcld/Module.h>
#include <mcld/LD/LDSection.h>
#include <mcld/MC/Input.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/BranchIslandFactory.h>
#include <mcld/LD/Resolver.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/RelocationFactory.h>
#include <mcld/LD/RelocData.h>
#include <mcld/LD/Relocator.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/Fragment/Relocation.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// FragmentLinker
//===----------------------------------------------------------------------===//
/// Constructor
FragmentLinker::FragmentLinker(const LinkerConfig& pConfig,
                               Module& pModule,
                               TargetLDBackend& pBackend)

  : m_Config(pConfig),
    m_Module(pModule),
    m_Backend(pBackend) {
}

/// Destructor
FragmentLinker::~FragmentLinker()
{
}
