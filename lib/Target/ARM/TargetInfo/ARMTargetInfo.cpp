/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Target/TargetMachine.h>
#include <mcld/Target/TargetRegistry.h>

namespace mcld {

mcld::Target TheARMTarget;

extern "C" void LLVMInitializeARMLDTargetInfo() {
  // register into mcld::TargetRegistry
  mcld::RegisterTarget X(TheARMTarget, "arm" );
}

} // namespace of mcld

