/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCLDFile.h>
#include <mcld/MC/MCLDContext.h>
#include <cstring>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// MCLDFile
MCLDFile::MCLDFile()
  : m_Type(Unknown), m_pContext(0), m_Path(), m_InputName(0) {
}

MCLDFile::MCLDFile(llvm::StringRef pName,
                   const sys::fs::Path& pPath,
                   unsigned int pType)
  : m_Type(pType), m_pContext(0), m_Path(pPath) {
  m_InputName = (char*)malloc(sizeof(pName.size()));
  strncpy(m_InputName, pName.data(), pName.size());
}

MCLDFile::~MCLDFile()
{
  if (m_pContext)
    delete m_pContext;
  if (m_InputName)
    free(m_InputName);
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

