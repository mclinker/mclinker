//===- ArchiveReader.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARCHIVE_READER_INTERFACE_H
#define MCLD_ARCHIVE_READER_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/LD/LDReader.h"

namespace mcld
{

class Input;
class InputTree;

/** \class ArchiveReader
 *  \brief ArchiveReader provides an common interface for all archive readers.
 *
 *  ArchiveReader also reads the target-independent parts of an archive file.
 */
class ArchiveReader : public LDReader
{
public:
  ArchiveReader();
  virtual ~ArchiveReader();

  virtual InputTree *readArchive(Input &input) = 0;
};

} // namespace of mcld

#endif

