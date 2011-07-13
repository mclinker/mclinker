/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCLDFile.h>
#include <mcld/MC/MCLDContext.h>

using namespace mcld;

//==========================
// MCLDFile
MCLDFile::MCLDFile()
  : m_pContext(0) {
}

MCLDFile::~MCLDFile()
{
  if (m_pContext)
	  delete m_pContext;
}

bool MCLDFile::isRecognized() const
{
  return m_pContext;
}

