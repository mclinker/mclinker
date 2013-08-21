//===- MipsDiagnostic.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/TargetRegistry.h>
#include <mcld/LD/DWARFLineInfo.h>
#include "Mips.h"

namespace {

//===----------------------------------------------------------------------===//
// createMipsDiagnostic - the help function to create corresponding
// MipsDiagnostic
//===----------------------------------------------------------------------===//
mcld::DiagnosticLineInfo* createMipsDiagLineInfo(const mcld::Target& pTarget,
                                                 const std::string &pTriple)
{
  return new mcld::DWARFLineInfo();
}

} // namespace of mcld

//===----------------------------------------------------------------------===//
// InitializeMipsDiagnostic
//===----------------------------------------------------------------------===//
extern "C" void MCLDInitializeMipsDiagnosticLineInfo() {
  mcld::TargetRegistry::RegisterDiagnosticLineInfo(mcld::TheMipselTarget,
                                                   createMipsDiagLineInfo);
  mcld::TargetRegistry::RegisterDiagnosticLineInfo(mcld::TheMips64elTarget,
                                                   createMipsDiagLineInfo);
}
