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
SectLinker::SectLinker(llvm::AsmPrinter& pPrinter, TargetLDBackend& pLDBackend)
  : MachineFunctionPass(m_ID), m_AsmPrinter(pPrinter), m_LDBackend(pLDBackend) {
  m_pLDDriver = new MCLDDriver(pLDBackend);
}

SectLinker::~SectLinker()
{
  delete m_pLDDriver;
}

bool SectLinker::doInitialization(Module &pM)
{
  cerr << "initialize! ho ho !" << endl;
}

bool SectLinker::doFinalization(Module &pM)
{
  cerr << "finalization! ho ho !" << endl;
}

bool SectLinker::runOnMachineFunction(MachineFunction& pF)
{
  cerr << "a machine function! ha ha!" << endl;
}

