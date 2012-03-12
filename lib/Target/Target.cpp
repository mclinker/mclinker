//===- Target.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/TargetRegistry.h"
#include "mcld/Target/TargetMachine.h"
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>

using namespace llvm;
using namespace mcld;

/* ** */
mcld::Target::Target()
  : TargetMachineCtorFn(0),
    SectLinkerCtorFn(0),
    TargetLDBackendCtorFn(0),
    m_pT(0)
{
}

