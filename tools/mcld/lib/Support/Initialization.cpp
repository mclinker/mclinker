//===- Initialization.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "alone/Config/Config.h"
#include "alone/Support/Initialization.h"

void alone::init::Initialize() {
  static bool is_initialized = false;
  if (is_initialized) {
    return;
  }

#if defined(PROVIDE_ARM_CODEGEN)
#endif

#if defined(PROVIDE_MIPS_CODEGEN)
#endif

#if defined(PROVIDE_X86_CODEGEN)
#endif

  is_initialized = true;

  return;
}

