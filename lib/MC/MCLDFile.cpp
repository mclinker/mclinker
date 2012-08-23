//===- MCLDFile.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/MCLDFile.h>
#include <mcld/LD/LDContext.h>
#include <cstring>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// MCLDFile
//===----------------------------------------------------------------------===//
MCLDFile::MCLDFile()
  : m_pContext(NULL), m_Path() {
}

MCLDFile::MCLDFile(const sys::fs::Path& pPath)
  : m_pContext(NULL), m_Path(pPath) {
}

MCLDFile::~MCLDFile()
{
}

