//===- ARMELFObjectWriter.h -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARMELFOBJECTWRITER_H
#define ARMELFOBJECTWRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/MC/MCELFObjectTargetWriter.h"

namespace mcld
{

/** \class ARMELFObjectWriter
 *  \brief ARMELFObjectWriter writes target-dependent parts of ELF object file.
 *
 *  \see
 */
class ARMELFObjectWriter : public MCELFObjectTargetWriter
{

};

} // namespace of mcld

#endif

