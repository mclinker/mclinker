//===- ARMELFObjectReader.h -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARMELFOBJECTREADER_H
#define ARMELFOBJECTREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/MC/MCELFObjectTargetReader.h"

namespace mcld
{

/// ARMELFObjectReader - ARMELFObjectReader is
/// a target-dependent ELF object reader.
class ARMELFObjectReader : public MCELFObjectTargetReader
{
};

} // namespace of mcld

#endif

