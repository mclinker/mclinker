//===- TextDiagnosticPrinter.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TEXT_DIAGNOSTIC_PRINTER_H
#define MCLD_TEXT_DIAGNOSTIC_PRINTER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/DiagnosticPrinter.h>
#include <llvm/Support/raw_ostream.h>

namespace mcld
{

class MCLDInfo;

/** \class TextDiagnosticPrinter
 *  \brief The plain, text-based DiagnosticPrinter.
 */
class TextDiagnosticPrinter : public DiagnosticPrinter
{
public:
  TextDiagnosticPrinter(llvm::raw_ostream& pOStream, const MCLDInfo& pLDInfo);

  virtual ~TextDiagnosticPrinter();

  /// HandleDiagnostic - Handle this diagnostic, reporting it to the user or
  /// capturing it to a log as needed.
  virtual void handleDiagnostic(DiagnosticEngine::Severity pSeverity,
                                const Diagnostic& pInfo);

  virtual void beginInput(const Input& pInput, const MCLDInfo& pLDInfo);

  virtual void endInput();

private:
  llvm::raw_ostream& m_OStream;
  const MCLDInfo& m_LDInfo;
  const Input* m_pInput;
};

} // namespace of mcld

#endif

