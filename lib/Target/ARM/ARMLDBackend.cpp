/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Target/TargetRegistry.h>
#include "ARM.h"
#include "ARMLDBackend.h"

using namespace mcld;

//=============================
// Force static initialization.
extern "C" void LLVMInitializeARMLDBackend() {
  // Register the linker backend
  RegisterLDBackend<ARMLDBackend> A(TheARMTarget);
}


