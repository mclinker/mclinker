/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCArchiveReader.h>
#include <mcld/Target/TargetArchiveReader.h>

using namespace mcld;

//==========================
// MCArchiveReader

MCArchiveReader::MCArchiveReader(TargetLDBackend::TargetArchiveReaderCtorFnTy pCtorFn)
  : m_pReader(pCtorFn()) {
}

MCArchiveReader::~MCArchiveReader()
{
  delete m_pReader;
}
