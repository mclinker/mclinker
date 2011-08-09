/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLD_GNU_ARCHIVE_READER_H
#define MCLD_GNU_ARCHIVE_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCArchiveReader.h>

namespace mcld
{

/** \class MCGNUArchiveReader
 *  \brief MCGNUArchiveReader reads the target-independent parts of an archive file.
 *
 *  \see
 *  \author Duo <pinronglu@gmail.com>
 */
class MCGNUArchiveReader : public MCArchiveReader
{
public:
  MCGNUArchiveReader();
  ~MCGNUArchiveReader();
};

} // namespace of mcld

#endif

