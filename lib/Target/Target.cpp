/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@gmail.com>                                          *
 ****************************************************************************/
#include <mcld/Target/TargetRegistry.h>
#include <mcld/Target/TargetMachine.h>
#include <llvm/Target/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>

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

