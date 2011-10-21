//===- X86ELFArchiveReader.h ----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86ELFARCHIVEREADER_H
#define X86ELFARCHIVEREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/MC/MCELFArchiveTargetReader.h"

namespace mcld
{

/** \class X86ELFArchiveReader
 *  \brief X86ELFArchiveReader is a target-dependent reader for ELF archive files.
 *
 *  \see
 */
class X86ELFArchiveReader : public MCELFArchiveTargetReader
{
};


} // namespace of mcld

#endif

