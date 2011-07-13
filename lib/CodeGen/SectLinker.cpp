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
#include <llvm/MC/MCObjectWriter.h>
#include <llvm/CodeGen/MachineFunction.h>
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDDriver.h>
#include <mcld/MC/MCLDFile.h>
#include <mcld/MC/MCLDInfo.h>

#ifdef MCLD_DEBUG
#include <iostream>
using namespace std;
#endif

using namespace mcld;
using namespace llvm;

char SectLinker::m_ID = 0;

//===----------------------------------------------------------------------===//
// SectLinker
SectLinker::SectLinker(TargetLDBackend& pLDBackend, MCLDFile* pDefaultBitcode)
  : MachineFunctionPass(m_ID),
    m_pLDBackend(&pLDBackend),
    m_pDefaultBitcode(pDefaultBitcode) {
}

SectLinker::~SectLinker()
{
  delete m_pLDBackend;
  delete m_pLDDriver;
  delete m_pDefaultBitcode;
}

bool SectLinker::doInitialization(Module &pM)
{
  // create MCLDInfo
  MCLDInfo *ldInfo = createLDInfo();
  if (0 != m_pDefaultBitcode)
    ldInfo->setDefaultBitcode(*m_pDefaultBitcode);

  // set up all target-independent parameters into MCLDInfo

  m_pLDDriver = new MCLDDriver(*ldInfo, *m_pLDBackend);
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

