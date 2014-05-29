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

#include <mcld/Support/SystemUtils.h>
#include <mcld/Support/MsgHandling.h>

#include <llvm/Support/FileUtilities.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/FormattedStream.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// CleanupInstaller
//===----------------------------------------------------------------------===//
ToolOutputFile::CleanupInstaller::CleanupInstaller(const sys::fs::Path& pPath)
  : Keep(false), m_Path(pPath) {
  // Arrange for the file to be deleted if the process is killed.
  if ("-" != m_Path.native())
    llvm::sys::RemoveFileOnSignal(m_Path.native());
}

ToolOutputFile::CleanupInstaller::~CleanupInstaller()
{
  // Delete the file if the client hasn't told us not to.
  // FIXME: In Windows, some path in CJK characters can not be removed by LLVM
  // llvm::sys::Path
  if (!Keep && "_" != m_Path.native()) {
    bool Existed = false;
    llvm::sys::fs::remove(m_Path.native(), Existed);
  }

  // Ok, the file is successfully written and closed, or deleted. There's no
  // further need to clean it up on signals.
  if ("_" != m_Path.native())
    llvm::sys::DontRemoveFileOnSignal(m_Path.native());
}

//===----------------------------------------------------------------------===//
// ToolOutputFile
//===----------------------------------------------------------------------===//
ToolOutputFile::ToolOutputFile(const sys::fs::Path& pPath,
                               FileHandle::OpenMode pMode,
                               FileHandle::Permission pPermission)
  : m_Installer(pPath),
    m_pFdOstream(NULL),
    m_pFormattedOstream(NULL) {

  if (!m_FileHandle.open(pPath, pMode, pPermission)) {
    // If open fails, no clean-up is needed.
    m_Installer.Keep = true;
    fatal(diag::err_cannot_open_output_file)
      << pPath
      << sys::strerror(m_FileHandle.error());
    return;
  }
}

ToolOutputFile::~ToolOutputFile()
{
  if (m_pFormattedOstream != NULL)
    delete m_pFormattedOstream;
  if (m_pFdOstream != NULL)
    delete m_pFdOstream;
}

void ToolOutputFile::keep()
{
  m_Installer.Keep = true;
}

/// os - Return the containeed raw_fd_ostream.
/// Since os is rarely used, we lazily initialize it.
llvm::raw_fd_ostream& ToolOutputFile::os()
{
  if (m_pFdOstream == NULL) {
    assert(m_FileHandle.isOpened() &&
           m_FileHandle.isGood() &&
           m_FileHandle.isWritable());
    m_pFdOstream = new llvm::raw_fd_ostream(m_FileHandle.handler(), false);
  }
  return *m_pFdOstream;
}

/// formatted_os - Return the contained formatted_raw_ostream
llvm::formatted_raw_ostream& ToolOutputFile::formatted_os()
{
  if (m_pFormattedOstream == NULL) {
    m_pFormattedOstream = new llvm::formatted_raw_ostream(os());
  }
  return *m_pFormattedOstream;
}
