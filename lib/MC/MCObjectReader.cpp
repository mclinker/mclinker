/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCObjectReader.h>

using namespace mcld;

//==========================
// MCObjectReader

MCObjectReader::MCObjectReader(TargetLDBackend::TargetObjectReaderCtorFnTy pCtorFn)
  : m_pReader(pCtorFn()) {
}

MCObjectReader::~MCObjectReader()
{
  delete m_pReader;
}
