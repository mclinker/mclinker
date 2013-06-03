//===- MipsTargetInfo.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/TargetMachine.h"
#include "mcld/Support/TargetRegistry.h"

namespace mcld {

mcld::Target TheMipselTarget;
mcld::Target TheMips64elTarget;

extern "C" void MCLDInitializeMipsLDTargetInfo() {
  mcld::RegisterTarget X1(TheMipselTarget, "mipsel");
  mcld::RegisterTarget X2(TheMips64elTarget, "mips64el");
}

} // namespace of mcld
