//===- DiagnosticInfo.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_DIAGNOSTIC_INFORMATION_H
#define MCLD_DIAGNOSTIC_INFORMATION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

namespace diag {
  enum ID {
#define DIAG(ENUM, CLASS, ADDRMSG, LINEMSG) ENUM,
#include "mcld/LD/DiagCommonKinds.inc"
#include "mcld/LD/DiagSymbolResolutions.inc"
    NUM_OF_BUILDIN_DIAGNOSTIC_INFO
  };
} // namespace of diag

/** \class DiagnosticInfo
 *  \brief 
 */
class DiagnosticInfo
{

};

} // namespace of mcld

#endif

