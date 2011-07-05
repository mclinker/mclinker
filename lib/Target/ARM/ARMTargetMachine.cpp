/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Target/TargetMachine.h>
#include <mcld/Target/TargetRegistry.h>
#include "ARM.h"

extern "C" void LLVMInitializeARMLDTarget() {
  // Register createTargetMachine function pointer to mcld::Target
  mcld::RegisterTargetMachine<mcld::LLVMTargetMachine> X(mcld::TheARMTarget);
}

