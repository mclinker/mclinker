//===- ToolOutputFile.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/ToolOutputFile.h>

#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/FileHandle.h>

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
ToolOutputFile::ToolOutputFile(MemoryArea& pMemoryArea)
  : m_OStream(pMemoryArea), m_pInstaller(NULL) {
  if (NULL != pMemoryArea.handler()) {
    m_pInstaller = new CleanupInstaller(pMemoryArea.handler()->path().native());
    m_pInstaller->Keep = true;
  }
}

void ToolOutputFile::keep()
{
  if (NULL != m_pInstaller) {
    m_pInstaller->Keep = true;
  }
}

