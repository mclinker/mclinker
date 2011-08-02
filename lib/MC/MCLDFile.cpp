/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCLDFile.h>
#include <mcld/MC/MCLDContext.h>
#include <mcld/Support/FileSystem.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace std;

using namespace mcld;

//===----------------------------------------------------------------------===//
// MCLDFile
MCLDFile::MCLDFile()
  : m_Type(Unknown), m_pContext(0), m_Path(), m_InputName() {
}

MCLDFile::MCLDFile(llvm::StringRef pName,
                   const sys::fs::Path& pPath,
                   unsigned int pType)
  : m_Type(pType), m_pContext(0), m_Path(pPath), m_InputName(pName.data()) {
}

MCLDFile::~MCLDFile()
{
  if (m_pContext)
    delete m_pContext;
}

llvm::StringRef MCLDFile::name() const
{
  return llvm::StringRef(m_InputName);
}

//===----------------------------------------------------------------------===//
// MCLDFileFactory
MCLDFile* MCLDFileFactory::produce(llvm::StringRef pName,
                                   const sys::fs::Path& pPath,
                                   unsigned int pType)
{
    MCLDFile* result = Alloc::allocate();
    new (result) MCLDFile(pName, pPath, pType);
    ++m_NumCreatedFiles;
    return result;
}

MCLDFile* MCLDFileFactory::produce()
{
    MCLDFile* result = Alloc::allocate();
    new (result) MCLDFile();
    ++m_NumCreatedFiles;
    return result;
}

