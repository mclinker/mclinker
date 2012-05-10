//===- TextDiagnosticPrinter.cpp ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/TextDiagnosticPrinter.h>
#include <mcld/MC/MCLDInfo.h>
#include <llvm/Support/Signals.h>
#include <string>

using namespace mcld;
 
static const enum llvm::raw_ostream::Colors UnreachableColor = llvm::raw_ostream::RED;
static const enum llvm::raw_ostream::Colors FatalColor       = llvm::raw_ostream::YELLOW;
static const enum llvm::raw_ostream::Colors ErrorColor       = llvm::raw_ostream::RED;
static const enum llvm::raw_ostream::Colors WarningColor     = llvm::raw_ostream::MAGENTA;
static const enum llvm::raw_ostream::Colors DebugColor       = llvm::raw_ostream::CYAN;
static const enum llvm::raw_ostream::Colors NoteColor        = llvm::raw_ostream::GREEN;
static const enum llvm::raw_ostream::Colors IgnoreColor      = llvm::raw_ostream::BLUE;

// Used for changing only the bold attribute.
static const enum llvm::raw_ostream::Colors SavedColor = llvm::raw_ostream::SAVEDCOLOR;

//===----------------------------------------------------------------------===//
// TextDiagnosticPrinter
TextDiagnosticPrinter::TextDiagnosticPrinter(llvm::raw_ostream& pOStream,
                                             const MCLDInfo& pLDInfo)
  : m_OStream(pOStream), m_LDInfo(pLDInfo) {
}

TextDiagnosticPrinter::~TextDiagnosticPrinter()
{
}

/// HandleDiagnostic - Handle this diagnostic, reporting it to the user or
/// capturing it to a log as needed.
void
TextDiagnosticPrinter::handleDiagnostic(DiagnosticEngine::Severity pSeverity,
                                        const Diagnostic& pInfo)
{
  DiagnosticPrinter::handleDiagnostic(pSeverity, pInfo);

  std::string out_string;
  pInfo.format(out_string);

  switch (pSeverity) {
    case DiagnosticEngine::Unreachable: {
      m_OStream.changeColor(UnreachableColor, true);
      m_OStream << "Unreachable: ";
      break;
    }
    case DiagnosticEngine::Fatal: {
      m_OStream.changeColor(FatalColor, true);
      m_OStream << "Fatal: ";
      break;
    }
    case DiagnosticEngine::Error: {
      m_OStream.changeColor(ErrorColor, true);
      m_OStream << "Error: ";
      break;
    }
    case DiagnosticEngine::Warning: {
      m_OStream.changeColor(WarningColor, true);
      m_OStream << "Warning: ";
      break;
    }
    case DiagnosticEngine::Debug: {
      // show debug message only if --verbose[=0]
      if (0 == m_LDInfo.options().verbose()) {
        m_OStream.changeColor(DebugColor, true);
        m_OStream << "Debug: ";
      }
      break;
    }
    case DiagnosticEngine::Note: {
      // show ignored message only if --verbose=1
      if (1 == m_LDInfo.options().verbose()) {
        m_OStream.changeColor(NoteColor, true);
        m_OStream << "Note: ";
      }
      break;
    }
    case DiagnosticEngine::Ignore: {
      // show ignored message only if --verbose=2
      if (2 == m_LDInfo.options().verbose()) {
        m_OStream.changeColor(IgnoreColor, true);
        m_OStream << "Ignore: ";
      }
      break;
    }
    default:
      break;
  }

  // print out the real message
  m_OStream.resetColor();
  m_OStream << out_string;

  switch (pSeverity) {
    case DiagnosticEngine::Unreachable: {
      m_OStream << "\n\n";
      m_OStream.changeColor(llvm::raw_ostream::YELLOW);
      m_OStream << "You encounter a bug of MCLinker, please report to:\n"
                << "  mclinker@googlegroups.com\n";
      m_OStream.resetColor();
    }
    /** fall through **/
    case DiagnosticEngine::Fatal: {
      // If we reached here, we are failing ungracefully. Run the interrupt handlers
      // to make sure any special cleanups get done, in particular that we remove
      // files registered with RemoveFileOnSignal.
      llvm::sys::RunInterruptHandlers();
      exit(1);
      break;
    }
    case DiagnosticEngine::Error: {
      if (m_LDInfo.options().maxErrorNum() != -1 &&
          getNumErrors() > m_LDInfo.options().maxErrorNum()) {
        m_OStream << "\n\n";
        m_OStream.changeColor(llvm::raw_ostream::YELLOW);
        m_OStream << "too many error messages (>"
                  << m_LDInfo.options().maxErrorNum() << ")...\n";
	m_OStream.resetColor();
        llvm::sys::RunInterruptHandlers();
        exit(1);
      }
      break;
    }
    case DiagnosticEngine::Warning: {
      if (m_LDInfo.options().maxWarnNum() != -1 &&
          getNumWarnings() > m_LDInfo.options().maxWarnNum()) {
        m_OStream << "\n\n";
        m_OStream.changeColor(llvm::raw_ostream::YELLOW);
        m_OStream << "too many warning messages (>"
                  << m_LDInfo.options().maxWarnNum() << ")...\n";
	m_OStream.resetColor();
        llvm::sys::RunInterruptHandlers();
        exit(1);
      }
    }
    default:
      break;
  }
  m_OStream << "\n";
}

