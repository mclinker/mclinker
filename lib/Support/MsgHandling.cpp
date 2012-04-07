//===- MsgHandling.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDDiagnostic.h>
#include <mcld/Support/MsgHandling.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/raw_ostream.h>

using namespace mcld;

//==========================
// MsgHandlerInitializer
class MsgHandlerInitializer : public llvm::ManagedStaticBase
{
public:
  MsgHandler* initialize(LDDiagnostic& pDiagnostic,
                  llvm::raw_ostream& pOStream)
  {
    RegisterManagedStatic(NULL, llvm::object_deleter<MsgHandler>::call);
    if (llvm::llvm_is_multithreaded()) {
      llvm::llvm_acquire_global_lock();

      void* tmp = new MsgHandler(pDiagnostic, pOStream);

      TsanHappensBefore(this);
      llvm::sys::MemoryFence();
      TsanIgnoreWritesBegin();
      Ptr = tmp;
      TsanIgnoreWritesEnd();
      llvm::llvm_release_global_lock();
    }
    else {
      Ptr = new MsgHandler(pDiagnostic, pOStream);
    }
    return static_cast<MsgHandler*>(Ptr);
  }
};

static MsgHandlerInitializer g_MsgHandlerInitializer;
static MsgHandler* g_pMsgHandler = NULL;

void mcld::InitializeMsgHandler(LDDiagnostic& pDiagnostic,
                          llvm::raw_ostream& pOStream)
{
  if (NULL == g_pMsgHandler) {
    g_pMsgHandler = g_MsgHandlerInitializer.initialize(pDiagnostic,
                                                       pOStream);
  }
}

MsgHandler& mcld::getGlobalMsgHandler()
{
  return *g_pMsgHandler;
}

//==========================
// MsgHandling
MsgHandler& mcld::fatal()
{
  getGlobalMsgHandler() << MsgHandler::Fatal;
  return getGlobalMsgHandler();
}

MsgHandler& mcld::error()
{
  getGlobalMsgHandler() << MsgHandler::Error;
  return getGlobalMsgHandler();
}

MsgHandler& mcld::warning()
{
  getGlobalMsgHandler() << MsgHandler::Warning;
  return getGlobalMsgHandler();
}

MsgHandler& mcld::critical()
{
  getGlobalMsgHandler() << MsgHandler::Critical;
  return getGlobalMsgHandler();
}

MsgHandler& mcld::info()
{
  getGlobalMsgHandler() << MsgHandler::Info;
  return getGlobalMsgHandler();
}

MsgHandler& mcld::debug()
{
  getGlobalMsgHandler() << MsgHandler::Debug;
  return getGlobalMsgHandler();
}

MsgHandler& mcld::unreachable()
{
  getGlobalMsgHandler() << MsgHandler::Unreachable;
  return getGlobalMsgHandler();
}

