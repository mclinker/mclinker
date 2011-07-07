/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCARCHIVEREADER_H
#define MCARCHIVEREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCArchiveReader
 *  \brief MCArchiveReader provides the interface of reading an archive.
 *
 *  \see TargetArchiveReader
 *  \author Duo <pinronglu@gmail.com>
 */
class MCArchiveReader
{
protected:
  MCArchiveReader();
public:
  virtual ~MCArchiveReader();
};

} // namespace of mcld

#endif

