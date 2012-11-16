//===- Environment.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Environment.h>
#include <mcld/Support/TargetSelect.h>

#include <mcld/Support/MsgHandling.h>

void mcld::Initialize(const LinkerConfig& pConfig)
{
  static bool is_initialized = false;

  if (is_initialized)
    return;

  mcld::InitializeAllTargets();
  mcld::InitializeAllEmulations();
  mcld::InitializeAllDiagnostics();
  mcld::InitializeDiagnosticEngine(pConfig);

  is_initialized = true;
}

void mcld::Finalize()
{
  mcld::FinalizeDiagnosticEngine();
}

