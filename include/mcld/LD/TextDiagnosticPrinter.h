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

namespace mcld
{

/** \class TextDiagnosticPrinter
 *  \brief The plain, text-based DiagnosticPrinter.
 */
class TextDiagnosticPrinter : public DiagnosticPrinter
{

};

} // namespace of mcld

#endif

