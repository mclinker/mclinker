//===- ARMDiagnostic.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_DIAGNOSTIC_H
#define MCLD_ARM_DIAGNOSTIC_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/LDDiagnostic.h>

namespace mcld {

class ARMDiagnostic : public LDDiagnostic
{
};

} // namespace of mcld

#endif

