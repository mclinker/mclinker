/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCELFArchiveReader.h>

using namespace mcld;

//==========================
// MCELFArchiveReader
MCELFArchiveReader::MCELFArchiveReader(TargetLDBackend::TargetArchiveReaderCtorFnTy pCtorFn)
  : MCArchiveReader(pCtorFn) {
}

MCELFArchiveReader::~MCELFArchiveReader()
{
}
 
