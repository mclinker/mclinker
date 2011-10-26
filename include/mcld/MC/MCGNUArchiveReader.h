//===- MCGNUArchiveReader.h -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_GNU_ARCHIVE_READER_H
#define MCLD_GNU_ARCHIVE_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/MC/MCArchiveReader.h"

namespace mcld
{

/// MCGNUArchiveReader - MCGNUArchiveReader reads
/// the target-independent parts of an archive file.
class MCGNUArchiveReader : public MCArchiveReader
{
public:
  MCGNUArchiveReader();
  ~MCGNUArchiveReader();
};

} // namespace of mcld

#endif

