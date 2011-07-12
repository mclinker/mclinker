/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <llvm/ADT/Triple.h>
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/Target/TargetRegistry.h>
#include "ARM.h"
#include "ARMELFSectLinker.h"

using namespace mcld;

namespace mcld {
//===----------------------------------------------------------------------===//
/// createARMSectLinker - the help funtion to create corresponding ARMSectLinker
///
SectLinker* createARMSectLinker(const std::string &pTriple,
                                mcld::TargetLDBackend &pLDBackend)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker has not supported yet");
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker has not supported yet");
  }
  return new ARMELFSectLinker(pLDBackend);
}

} // namespace of mcld

//==========================
// ARMSectLinker
extern "C" void LLVMInitializeARMSectLinker() {
  // Register the linker frontend
  mcld::TargetRegistry::RegisterSectLinker(TheARMTarget, createARMSectLinker);
}

