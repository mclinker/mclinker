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
 *  There are some property on all the archive formats.
 *  1. All archive elements star on an even boundary, new line padded;
 *  2. All archive headers are char *;
 *  3. All archive headers are the same size.
 */

class ArchiveReader : public LDReader
{
protected:
  struct ArchiveMemberHeader
  {
    char name[16];
    char date[12];
    char uid[6];
    char gid[6];
    char mode[8];
    char size[10];
    char finalMagic[2];
  };

public:
  ArchiveReader();
  virtual ~ArchiveReader();

  virtual InputTree *readArchive(Input &input) = 0;
};

} // namespace of mcld

#endif

