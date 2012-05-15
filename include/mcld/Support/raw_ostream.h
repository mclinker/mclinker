//===- raw_ostream.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_RAW_OSTREAM_H
#define MCLD_RAW_OSTREAM_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <llvm/Support/raw_ostream.h>
#include <mcld/MC/MCLDInfo.h>

namespace mcld
{

class raw_fd_ostream : public llvm::raw_fd_ostream
{
public:
  /// raw_fd_ostream - Open the specified file for writing. If an error occurs,
  /// information about the error is put into ErrorInfo, and the stream should
  /// be immediately destroyed; the string will be empty if no error occurred.
  /// This allows optional flags to control how the file will be opened.
  ///
  /// As a special case, if Filename is "-", then the stream will use
  /// STDOUT_FILENO instead of opening a file. Note that it will still consider
  /// itself to own the file descriptor. In particular, it will close the
  /// file descriptor when it is done (this is necessary to detect
  /// output errors).
  raw_fd_ostream(const char *pFilename,
                 std::string &pErrorInfo,
                 unsigned int pFlags = 0,
                 const MCLDInfo* pLDInfo = NULL);

  /// raw_fd_ostream ctor - FD is the file descriptor that this writes to.  If
  /// ShouldClose is true, this closes the file when the stream is destroyed.
  raw_fd_ostream(int pFD, bool pShouldClose,
                 bool pUnbuffered=false,
                 const MCLDInfo* pLDInfo = NULL);

  virtual ~raw_fd_ostream();

  void setLDInfo(const MCLDInfo& pLDInfo);


  llvm::raw_ostream &changeColor(enum llvm::raw_ostream::Colors pColors,
                                 bool pBold=false,
                                 bool pBackground=false);

  llvm::raw_ostream &resetColor();

  // FIXME: migrate to newer LLVM
  // llvm::raw_ostream &reverseColor();

  bool is_displayed() const;

private:
  const MCLDInfo* m_pLDInfo;

};

/// InitializeOStreams - This initialize mcld::outs() and mcld::errs().
/// Call it before you use mcld::outs() and mcld::errs().
void InitializeOStreams(const MCLDInfo& pLDInfo);

/// outs() - This returns a reference to a raw_ostream for standard output.
/// Use it like: outs() << "foo" << "bar";
mcld::raw_fd_ostream &outs();

/// errs() - This returns a reference to a raw_ostream for standard error.
/// Use it like: errs() << "foo" << "bar";
mcld::raw_fd_ostream &errs();

} // namespace of mcld

#endif

