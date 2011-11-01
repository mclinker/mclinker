//===- ARMELFArchiveReader.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARMELFARCHIVEREADER_H
#define ARMELFARCHIVEREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/MC/MCELFArchiveTargetReader.h"

namespace mcld
{

/// ARMELFArchiveReader - ARMELFArchiveReader is
/// a target-dependent reader for ELF archive files.
class ARMELFArchiveReader : public MCELFArchiveTargetReader
{
};

} // namespace of mcld

#endif

