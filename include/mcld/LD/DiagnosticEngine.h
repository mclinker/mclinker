//===- DiagnosticEngine.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_DIAGNOSTIC_ENGINE_H
#define MCLD_DIAGNOSTIC_ENGINE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/MsgHandler.h>

namespace mcld
{

class MCLDInfo;
class DiagnosticPrinter;
class DiagnosticLineInfo;

/** \class DiagnosticEngine
 *  \brief DiagnosticEngine is used to report problems and issues.
 *
 *  DiagnosticEngine is used to report problems and issues. It creates the
 *  Diagnostics and passes them to the DiagnosticPrinter for reporting to the
 *  user.
 *
 *  DiagnosticEngine is a complex class, it is responsible for
 *  - remember the argument string for MsgHandler
 *  - choice the severity of a message by options
 */
class DiagnosticEngine
{
public:
  enum Level {
    Unreachable,
    Fatal,
    Error,
    Warning,
    Debug,
    Note,
    Ignore,
    None
  };

public:
  DiagnosticEngine(const MCLDInfo& pLDInfo,
                   DiagnosticLineInfo* pLineInfo = NULL,
                   DiagnosticPrinter* pPrinter = NULL,
                   bool pShouldOwnPrinter = true);

  ~DiagnosticEngine();

  void setPrinter(DiagnosticPrinter& pPrinter, bool pShouldOwnPrinter = true);

  DiagnosticPrinter* getPrinter()
  { return m_pPrinter; }

  const DiagnosticPrinter* getPrinter() const
  { return m_pPrinter; }

  DiagnosticPrinter* takePrinter() {
    m_OwnPrinter = false;
    return m_pPrinter;
  }

  bool ownPrinter() const
  { return m_OwnPrinter; }

  // report - Issue the message to the printer.
  MsgHandler report(unsigned int pID, Level pLevel);

private:
  const MCLDInfo& m_LDInfo;
  DiagnosticLineInfo* m_pLineInfo;
  DiagnosticPrinter* m_pPrinter;
  bool m_OwnPrinter;
};

//===----------------------------------------------------------------------===//
// Inline member functions
inline MsgHandler DiagnosticEngine::report(unsigned int pID, Level pLevel)
{
  MsgHandler result(*this);
  return result;
}

} // namespace of mcld

#endif

