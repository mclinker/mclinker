/*****************************************************************************
 *   The MC Linker Project, Copyright (C), 2011 -                            *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@mediatek.com>                                         *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#include <llvm/Module.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/MachineFunction.h>
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDDriver.h>

#ifdef MCLD_DEBUG
#include <iostream>
using namespace std;
#endif

using namespace mcld;
using namespace llvm;

char SectLinker::m_ID = 0;

//===----------------------------------------------------------------------===//
// SectLinker
SectLinker::SectLinker(TargetLDBackend& pLDBackend)
  : MachineFunctionPass(m_ID), m_pLDBackend(&pLDBackend) {
  m_pLDDriver = new MCLDDriver(pLDBackend);
}

SectLinker::~SectLinker()
{
  delete m_pLDBackend;
  delete m_pLDDriver;
}

bool SectLinker::doInitialization(Module &pM)
{
  // set up all parameters into MCLDInfo
  // create MCLinker, pass MCLDInfo as one of arguments
}

bool SectLinker::doFinalization(Module &pM)
{
  // get MCSectionData
  // real linking
  // emit object file
}

bool SectLinker::runOnMachineFunction(MachineFunction& pF)
{
}

