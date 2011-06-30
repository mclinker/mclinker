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
#include <llvm/MC/MCContext.h>

using namespace llvm;
using namespace mcld;

/* ** */
mcld::Target::Target(const llvm::Target& pTarget)
  : m_T(pTarget), TargetLDBackendCtorFn(0), MCLDDriverCtorFn(0) {
}

mcld::Target::~Target() {
  TargetLDBackendCtorFn = 0;
  MCLDDriverCtorFn = 0;
}

llvm::MCCodeEmitter *mcld::Target::createCodeEmitter(
                                     mcld::LLVMTargetMachine &TM,
                                     MCContext &Ctx) const {
  return m_T.createCodeEmitter(TM.getTM(), Ctx);
}
