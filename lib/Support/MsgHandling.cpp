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
// DiagnosticInitializer
class DiagnosticInitializer : public llvm::ManagedStaticBase
{
public:
  DiagnosticEngine* initialize(const MCLDInfo& pLDInfo,
                               DiagnosticLineInfo* pLineInfo,
                               DiagnosticPrinter* pPrinter)
  {
    RegisterManagedStatic(NULL, llvm::object_deleter<DiagnosticEngine>::call);
    if (llvm::llvm_is_multithreaded()) {
      llvm::llvm_acquire_global_lock();
      void* tmp = NULL;
      if (NULL != pPrinter)
        tmp = new DiagnosticEngine(pLDInfo, pLineInfo, pPrinter, false);
      else
        tmp = new DiagnosticEngine(pLDInfo, pLineInfo, NULL, false);

      TsanHappensBefore(this);
      llvm::sys::MemoryFence();
      TsanIgnoreWritesBegin();
      Ptr = tmp;
      TsanIgnoreWritesEnd();
      llvm::llvm_release_global_lock();
    }
    else {
      if (NULL != pPrinter)
        Ptr = new DiagnosticEngine(pLDInfo, pLineInfo, pPrinter, false);
      else
        Ptr = new DiagnosticEngine(pLDInfo, pLineInfo, NULL, false);
    }
    return static_cast<DiagnosticEngine*>(Ptr);
  }
};

static DiagnosticInitializer g_DiagInitializer;
static DiagnosticEngine* g_pDiagnosticEngine = NULL;

void mcld::InitializeDiagnosticEngine(const mcld::MCLDInfo& pLDInfo,
                                DiagnosticLineInfo* pLineInfo,
                                DiagnosticPrinter* pPrinter)
{
  if (NULL == g_pDiagnosticEngine) {
    g_pDiagnosticEngine = g_DiagInitializer.initialize(pLDInfo,
                                                       pLineInfo,
                                                       pPrinter);
  }
}

DiagnosticEngine& mcld::getDiagnosticEngine()
{
  assert(NULL != g_pDiagnosticEngine &&
         "mcld::InitializeDiagnostics() is not called");
  return *g_pDiagnosticEngine;
}

