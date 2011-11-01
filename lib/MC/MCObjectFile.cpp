//===- MCObjectFile.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCObjectFile.h"
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

