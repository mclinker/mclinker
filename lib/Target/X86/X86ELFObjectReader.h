//===- X86ELFObjectReader.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86_ELF_OBJECT_READER_H
#define X86_ELF_OBJECT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/MC/MCELFObjectTargetReader.h"

namespace mcld
{

/// class X86ELFObjectReader - X86ELFObjectReader is
/// a target-dependent ELF object reader.
class X86ELFObjectReader : public MCELFObjectTargetReader
{
};

} // end namespace mcld

#endif

