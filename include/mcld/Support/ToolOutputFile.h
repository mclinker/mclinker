//===- ToolOutputFile.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SUPPORT_TOOL_OUTPUT_FILE_H
#define MCLD_SUPPORT_TOOL_OUTPUT_FILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <string>
#include <mcld/Support/raw_mem_ostream.h>

namespace mcld {

/** \class ToolOutputFile
 *  \brief ToolOutputFile contains a raw_mem_ostream and adds extra new
 *  features:
 *   - The file is automatically deleted if the process is killed.
 *   - The file is automatically deleted when the TooOutputFile object is
 *     destoryed unless the client calls keep().
 */
class ToolOutputFile
{
public:
  ToolOutputFile(MemoryArea& pOutputFile);

  /// os - Return the contained raw_mem_ostream.
  raw_mem_ostream &os() { return m_OStream; }

  /// memory - Return the contained MemoryArea.
  MemoryArea& memory() { return m_OStream.getMemoryArea(); }

  /// keep - Indicate that the tool's job wrt this output file has been
  /// successful and the file should not be deleted.
  void keep();

private:
  class CleanupInstaller
  {
  public:
    explicit CleanupInstaller(const std::string& pFilename);

    ~CleanupInstaller();

    /// Keep - The flag which indicates whether we should not delete the file.
    bool Keep;

  private:
    std::string m_Filename;
  }; 

private:
  raw_mem_ostream m_OStream;
  CleanupInstaller* m_pInstaller;
};

} // namespace of mcld

#endif

