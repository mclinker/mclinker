//===- ToolOutputFile.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/ToolOutputFile.h>

#include <mcld/Support/Path.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/raw_mem_ostream.h>

#include <llvm/Support/Signals.h>
#include <llvm/Support/Path.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// CleanupInstaller
//===----------------------------------------------------------------------===//
ToolOutputFile::CleanupInstaller::CleanupInstaller(const std::string& pName)
  : m_Filename(pName), Keep(false) {
  // Arrange for the file to be deleted if the process is killed.
  if (m_Filename != "-")
    llvm::sys::RemoveFileOnSignal(llvm::sys::Path(m_Filename));
}

ToolOutputFile::CleanupInstaller::~CleanupInstaller()
{
  // Delete the file if the client hasn't told us not to.
  if (!Keep && m_Filename != "-")
    llvm::sys::Path(m_Filename).eraseFromDisk();

  // Ok, the file is successfully written and closed, or deleted. There's no
  // further need to clean it up on signals.
  if (m_Filename != "-")
    llvm::sys::DontRemoveFileOnSignal(llvm::sys::Path(m_Filename));
}

//===----------------------------------------------------------------------===//
// ToolOutputFile
//===----------------------------------------------------------------------===//
ToolOutputFile::ToolOutputFile(const sys::fs::Path& pPath,
                               FileHandle::OpenMode pMode,
                               FileHandle::Permission pPermission)
  : m_Installer(pPath.native()),
    m_pMemoryArea(NULL),
    m_pOStream(NULL) {

  if (!m_FileHandle.open(pPath, pMode, pPermission)) {
    m_Installer.Keep = true;
    return;
  }

  m_pMemoryArea = new MemoryArea(m_FileHandle);
  m_pOStream = new raw_mem_ostream(*m_pMemoryArea);
}

ToolOutputFile::~ToolOutputFile()
{
  delete m_pMemoryArea;
  delete m_pOStream;
}

void ToolOutputFile::keep()
{
  m_Installer.Keep = true;
}

/// os - Return the contained raw_mem_ostream.
raw_mem_ostream& ToolOutputFile::os()
{
  assert(NULL != m_pOStream);
  return *m_pOStream;
}

/// memory - Return the contained MemoryArea.
MemoryArea& ToolOutputFile::memory()
{
  assert(NULL != m_pOStream);
  return m_pOStream->getMemoryArea();
}

