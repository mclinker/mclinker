//===- ToolOutputFile.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SUPPORT_TOOLOUTPUTFILE_H
#define MCLD_SUPPORT_TOOLOUTPUTFILE_H

#include <string>
#include <mcld/Support/FileHandle.h>

namespace llvm {
class formatted_raw_ostream;
} // end of namespace llvm

namespace mcld {

class Path;
class FileHandle;

/** \class ToolOutputFile
 *  \brief ToolOutputFile has the following features:
 *   - The file is automatically deleted if the process is killed.
 *   - The file is automatically deleted when the TooOutputFile object is
 *     destoryed unless the client calls keep().
 */
class ToolOutputFile
{
public:
  ToolOutputFile(const sys::fs::Path& pPath,
                 FileHandle::OpenMode pMode,
                 FileHandle::Permission pPermission);

  ~ToolOutputFile();

  /// fd - Retutn the output file handle
  FileHandle& fd() { return m_FileHandle; }

  /// os - Return the contained raw_fd_ostream
  llvm::raw_fd_ostream& os();

  /// formatted_os - Return the contained formatted_raw_ostream
  llvm::formatted_raw_ostream& formatted_os();

  /// keep - Indicate that the tool's job wrt this output file has been
  /// successful and the file should not be deleted.
  void keep();

private:
  class CleanupInstaller
  {
  public:
    explicit CleanupInstaller(const sys::fs::Path& pPath);

    ~CleanupInstaller();

    /// Keep - The flag which indicates whether we should not delete the file.
    bool Keep;

  private:
    sys::fs::Path m_Path;
  };

private:
  FileHandle m_FileHandle;
  CleanupInstaller m_Installer;
  llvm::raw_fd_ostream* m_pFdOstream;
  llvm::formatted_raw_ostream* m_pFormattedOstream;
};

} // namespace of mcld

#endif
