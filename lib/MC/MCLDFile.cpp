//===- MCLDFile.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCLDFile.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/Support/MemoryArea.h"
#include <cstring>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// MCLDFile
MCLDFile::MCLDFile()
  : m_Type(Unknown), m_pContext(0), m_Path(), m_Name(), m_pMemArea(0) {
}

MCLDFile::MCLDFile(llvm::StringRef pName)
  : m_Type(Unknown), m_pContext(0), m_Path(), m_Name(pName.data()), m_pMemArea(0) {
}

MCLDFile::MCLDFile(llvm::StringRef pName,
                   const sys::fs::Path& pPath,
                   unsigned int pType)
  : m_Type(pType), m_pContext(0), m_Path(pPath), m_Name(pName.data()), m_pMemArea(0) {
}

MCLDFile::~MCLDFile()
{
}

