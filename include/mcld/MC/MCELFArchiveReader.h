/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCELFARCHIVEREADER_H
#define MCELFARCHIVEREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCArchiveReader.h>
#include <mcld/Target/TargetLDBackend.h>

namespace mcld
{

/** \class MCELFArchiveReader
 *  \brief MCELFArchiveReader reads the target-independent parts of an archive file.
 *
 *  \see
 *  \author Duo <pinronglu@gmail.com>
 */
class MCELFArchiveReader : public MCArchiveReader
{
public:
  MCELFArchiveReader(TargetLDBackend::TargetArchiveReaderCtorFnTy pCtorFn);
  ~MCELFArchiveReader();
};

} // namespace of mcld

#endif

