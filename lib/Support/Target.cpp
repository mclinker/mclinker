//===- Target.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/Target.h>
#include <llvm/ADT/Triple.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Target
//===----------------------------------------------------------------------===//
Target::Target()
  : TargetMachineCtorFn(NULL),
    MCLinkerCtorFn(NULL),
    TargetLDBackendCtorFn(NULL),
    DiagnosticLineInfoCtorFn(NULL) {
}

unsigned int Target::getTripleQuality(const llvm::Triple& pTriple) const
{
  if (!TripleMatchQualityFn)
    return 0;
  return TripleMatchQualityFn(pTriple);
}

MCLDTargetMachine*
Target::createTargetMachine(const std::string& pTriple,
                            const llvm::Target& pTarget,
                            llvm::TargetMachine& pTM) const
{
  if (TargetMachineCtorFn)
      return TargetMachineCtorFn(pTarget, *this, pTM, pTriple);
  return NULL;
}

/// createMCLinker - create target-specific MCLinker
MCLinker*
Target::createMCLinker(const std::string &pTriple,
                       LinkerConfig& pConfig,
                       Module& pModule,
                       MemoryArea& pOutput) const
{
  if (!MCLinkerCtorFn)
    return NULL;
  return MCLinkerCtorFn(pTriple, pConfig, pModule, pOutput);
}

/// emulate - given MCLinker default values for the other aspects of the
/// target system.
bool Target::emulate(LinkerScript& pScript, LinkerConfig& pConfig) const
{
  if (!EmulationFn)
    return false;
  return EmulationFn(pScript, pConfig);
}

/// createLDBackend - create target-specific LDBackend
TargetLDBackend* Target::createLDBackend(const LinkerConfig& pConfig) const
{
    if (!TargetLDBackendCtorFn)
      return NULL;
    return TargetLDBackendCtorFn(pConfig);
}

/// createDiagnosticLineInfo - create target-specific DiagnosticLineInfo
DiagnosticLineInfo*
Target::createDiagnosticLineInfo(const mcld::Target& pTarget,
                                 const std::string& pTriple) const
{
  if (!DiagnosticLineInfoCtorFn)
    return NULL;
  return DiagnosticLineInfoCtorFn(pTarget, pTriple);
}

