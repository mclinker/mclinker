//===- MsgHandler.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_MESSAGE_HANDLER_H
#define MCLD_MESSAGE_HANDLER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <mcld/LD/DiagnosticInfo.h>

namespace mcld
{
class DiagnosticEngine;

/** \class MsgHandler
 *  \brief MsgHandler controls the timing to output message.
 */
class MsgHandler
{
public:
  
  MsgHandler(DiagnosticEngine& pDiagnostic);
  ~MsgHandler();

  // -----  Data Output Interfaces  ----- //
  void flush();

  MsgHandler &operator<<(llvm::StringRef pStr);
  MsgHandler &operator<<(const char* pStr);
  MsgHandler &operator<<(int pValue);
  MsgHandler &operator<<(unsigned int pValue);
  MsgHandler &operator<<(bool pValue);

private:
  DiagnosticEngine& m_Engine;
};

} // namespace of mcld

#endif

