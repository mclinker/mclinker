//===- MsgHandler.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/MsgHandler.h>
#include <mcld/LD/DiagnosticEngine.h>

using namespace mcld;

MsgHandler::MsgHandler(DiagnosticEngine& pEngine)
 : m_Engine(pEngine) {
}

MsgHandler::~MsgHandler()
{
  flush();
}

void MsgHandler::flush()
{
}

// -----  operator<<  ----- //
MsgHandler& MsgHandler::operator<<(llvm::StringRef pStr)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(const char* pStr)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(int pValue)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(unsigned int pValue)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(bool pValue)
{
  return *this;
}

