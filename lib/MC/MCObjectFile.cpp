/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCObjectFile.h>
#include "llvm/Support/system_error.h"

using namespace mcld;

//==========================
// MCObjectFile

MCObjectFile::MCObjectFile(llvm::StringRef FileName)
	: mFileName(FileName),
	  mpBuffer(NULL) {


}

MCObjectFile::~MCObjectFile()
{ }

