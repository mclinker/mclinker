/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCObjectFile.h>

using namespace mcld;

//==========================
// MCObjectFile

MCObjectFile::MCObjectFile(llvm::StringRef FileName)
	: mFileName(FileName)
{
	mp_Context = new MCLDContext();
}

MCObjectFile::~MCObjectFile()
{
	delete mp_Context;
}
