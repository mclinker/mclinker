/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <llvm/ADT/Triple.h>
#include <mcld/Target/TargetRegistry.h>
#include "ARM.h"
#include "ARMLDBackend.h"

using namespace mcld;

namespace mcld {

TargetLDBackend* createARMLDBackend(const llvm::Target& pTarget, 
                                    const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin())
    assert(0 && "MC Linker for Darwin has not been supported yet");
  if (theTriple.isOSWindows())
    assert(0 && "MC Linker for Windows has not been supported yet");
  assert(0 && "MC Linker for ELF system has not been supported yet");
  //return new ELFARMLDBackend(pTarget, pTriple);
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeARMLDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheARMTarget, createARMLDBackend);
}


