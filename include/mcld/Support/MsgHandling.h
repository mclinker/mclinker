//===- MsgHandling.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MESSAGE_HANDLING_H
#define MCLD_MESSAGE_HANDLING_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/MsgHandler.h>
#include <mcld/LD/LDDiagnostic.h>
#include <mcld/LD/DumbDiagnostic.h>

namespace mcld
{

void InitializeMsgHandler(LDDiagnostic& pDiagnostic,
                          llvm::raw_ostream& pOStream);

MsgHandler& getGlobalMsgHandler();

MsgHandler& fatal();
MsgHandler& error();
MsgHandler& warning();
MsgHandler& critical();
MsgHandler& info();
MsgHandler& debug();
MsgHandler& unreachable();

} // namespace of mcld

#endif

