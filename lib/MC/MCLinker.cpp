//===- MCLinker.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MCLinker class
//
//===----------------------------------------------------------------------===//

#include "mcld/MC/MCLinker.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLDInfo.h"
#include "mcld/LD/LDContext.h"
#include "mcld/Target/TargetLDBackend.h"
#include "llvm/Support/raw_ostream.h"

using namespace mcld;

MCLinker::MCLinker(TargetLDBackend& pBackend, MCLDInfo& pInfo)
: m_Backend(pBackend), m_Info(pInfo)
{
>>>>>>> Move MC/MCLDContext to LD/LDContext.
}

MCLinker::~MCLinker()
{
}

