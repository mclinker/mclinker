//===- MsgHandler.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/MC/MCAssembler.h>
#include <llvm/ADT/Twine.h>
#include <mcld/Support/LinkerErrors.h>
#include <mcld/Support/MsgHandler.h>
#include <mcld/LD/LDDiagnostic.h>
#include <mcld/MC/MCLDFile.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/Relocation.h>

using namespace mcld;

MsgHandler::MsgHandler(LDDiagnostic& pDiagnostic, llvm::raw_ostream& pOStream)
 : m_Level(Info), m_OStream(pOStream), m_Diagnostic(pDiagnostic) {
}

MsgHandler::~MsgHandler()
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

MsgHandler& MsgHandler::operator<<(const std::string& pStr)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(const llvm::Twine& pStr)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(LinkerError pError)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(MsgHandler::Level pLevle)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(const LDSymbol& pSymbol)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(const ResolveInfo& pSymbol)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(const Relocation& pRelocation)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(const llvm::MCFragment& pFragment)
{
  return *this;
}

MsgHandler& MsgHandler::operator<<(const MCLDFile& pFile)
{
  return *this;
}

