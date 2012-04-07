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
#include <llvm/Support/raw_ostream.h>
#include <mcld/Support/LinkerErrors.h>

namespace llvm
{
class MCFragment;
class Twine;
} // namespace of llvm

namespace mcld
{
class LDSymbol;
class ResolveInfo;
class Relocation;
class MCLDFile;
class LDDiagnostic;

/** \class MsgHandler
 *  \brief MsgHandler handles the output message.
 *  MsgHandler is a object adapter of llvm::raw_ostream
 */
class MsgHandler
{
public:
  enum Level
  {
    Fatal,
    Error,
    Warning,
    Critical,
    Info,
    Debug,
    Unreachable,
    NoLevel
  };

public:
  
  MsgHandler(LDDiagnostic& pDiagnostic, llvm::raw_ostream& pOStream);
  virtual ~MsgHandler();

  // -----  Data Output Interfaces  ----- //
  void flush();

  MsgHandler &operator<<(llvm::StringRef pStr);
  MsgHandler &operator<<(const char* pStr);
  MsgHandler &operator<<(const std::string& pStr);
  MsgHandler &operator<<(const llvm::Twine& pStr);
  MsgHandler &operator<<(LinkerError pError);
  MsgHandler &operator<<(Level pLevel);
  MsgHandler &operator<<(const LDSymbol& pSymbol);
  MsgHandler &operator<<(const ResolveInfo& pSymbol);
  MsgHandler &operator<<(const Relocation& pRelocation);
  MsgHandler &operator<<(const llvm::MCFragment& pFragment);
  MsgHandler &operator<<(const MCLDFile& pFile);

private:
  Level m_Level;
  llvm::raw_ostream& m_OStream;
  LDDiagnostic& m_Diagnostic;
};

} // namespace of mcld

#endif

