//===- Target.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/Target.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Target
//===----------------------------------------------------------------------===//
Target::Target()
  : TargetMachineCtorFn(NULL),
    MCLinkerCtorFn(NULL),
    TargetLDBackendCtorFn(NULL),
    DiagnosticLineInfoCtorFn(NULL),
    m_pT(NULL) {
}

