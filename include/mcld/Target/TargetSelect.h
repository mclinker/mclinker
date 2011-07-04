/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef TARGETSELECT_H
#define TARGETSELECT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

extern "C" {
  // Declare all of the available linker initialization functions.
#define LLVM_LINKER(TargetName) void LLVMInitialize##TargetName##Linker();
#include "mcld/Config/Linkers.def"
} // extern "C"

namespace mcld
{
  /// InitializeAllSectLinkers - The main program should call this function if it
  /// wants all linkers that LLVM is configured to support, to make them
  /// available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllSectLinkers() {
#define LLVM_LINKER(TargetName) LLVMInitialize##TargetName##Linker();
#include "mcld/Config/Linkers.def"
  }

} // namespace of mcld

#endif

