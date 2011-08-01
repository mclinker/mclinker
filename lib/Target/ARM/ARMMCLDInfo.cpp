/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include "ARM.h"
#include "ARMMCLDInfo.h"
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Target/TargetRegistry.h>
#include <llvm/ADT/Triple.h>

using namespace mcld;
using namespace llvm;

//===----------------------------------------------------------------------===//
/// ARMELFLDInfo - ARM ELF emulator of linker
///
ARMELFLDInfo::ARMELFLDInfo()
{
}

//===----------------------------------------------------------------------===//
/// createARMLDInfo - the help funtion to create corresponding ARMLDInfo
///
MCLDInfo* createARMLDInfo(const std::string &pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker has not supported yet");
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker has not supported yet");
  }
  // FIXME: Android ARM EABI may be different than ARM Linux ABI?
  return new ARMELFLDInfo();
}

//==========================
// ARMMCLDInfo
extern "C" void LLVMInitializeARMLDInfo() {
  // Register the linker information
  mcld::TargetRegistry::RegisterLDInfo(TheARMTarget, createARMLDInfo);
}

