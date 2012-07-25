//===- MsgHandling.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/DiagnosticEngine.h>
#include <mcld/LD/DiagnosticLineInfo.h>
#include <mcld/LD/DiagnosticPrinter.h>
#include <mcld/LD/MsgHandler.h>
#include <mcld/Support/MsgHandling.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/raw_ostream.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// static variables
//===----------------------------------------------------------------------===//
static llvm::ManagedStatic<DiagnosticEngine> g_pEngine;

void
mcld::InitializeDiagnosticEngine(const mcld::MCLDInfo& pLDInfo,
                                 DiagnosticLineInfo* pLineInfo,
                                 DiagnosticPrinter* pPrinter)
{
  g_pEngine->reset(pLDInfo);
  if (NULL != pLineInfo)
    g_pEngine->setLineInfo(*pLineInfo);

  if (NULL != pPrinter)
    g_pEngine->setPrinter(*pPrinter, false);
  else {
    DiagnosticPrinter* printer = new DiagnosticPrinter();
    g_pEngine->setPrinter(*printer, true);
  }
}

DiagnosticEngine& mcld::getDiagnosticEngine()
{
  return *g_pEngine;
}

