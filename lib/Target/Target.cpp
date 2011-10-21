//===- Target.cpp ---------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/TargetRegistry.h"
#include "mcld/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;
using namespace mcld;

/* ** */
mcld::Target::Target()
  : m_pT(0),
    TargetLDBackendCtorFn(0),
    TargetMachineCtorFn(0),
    SectLinkerCtorFn(0)
{
}

