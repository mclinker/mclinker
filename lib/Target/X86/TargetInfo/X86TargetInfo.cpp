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

mcld::Target TheX86Target;

extern "C" void LLVMInitializeX86LDTargetInfo() {
  // register into mcld::TargetRegistry
  mcld::RegisterTarget X(TheX86Target, "x86");
}

} // namespace of mcld

