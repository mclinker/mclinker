//===- raw_ostream.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/raw_ostream.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// raw_ostream
mcld::raw_fd_ostream::raw_fd_ostream(const char *pFilename,
                               std::string &pErrorInfo,
                               unsigned int pFlags,
                               const MCLDInfo* pLDInfo)
  : llvm::raw_fd_ostream(pFilename, pErrorInfo, pFlags), m_pLDInfo(pLDInfo) {
}

mcld::raw_fd_ostream::raw_fd_ostream(int pFD,
                               bool pShouldClose,
                               bool pUnbuffered,
                               const MCLDInfo* pLDInfo)
  : llvm::raw_fd_ostream(pFD, pShouldClose, pUnbuffered), m_pLDInfo(pLDInfo) {
}

mcld::raw_fd_ostream::~raw_fd_ostream()
{
}

void mcld::raw_fd_ostream::setLDInfo(const MCLDInfo& pLDInfo)
{
  m_pLDInfo = &pLDInfo;
}

llvm::raw_ostream &
mcld::raw_fd_ostream::changeColor(enum llvm::raw_ostream::Colors pColor,
                                  bool pBold,
                                  bool pBackground)
{
  if (!is_displayed())
    return *this;
  return llvm::raw_fd_ostream::changeColor(pColor, pBold, pBackground);
}

llvm::raw_ostream& mcld::raw_fd_ostream::resetColor()
{
  if (!is_displayed())
    return *this;
  return llvm::raw_fd_ostream::resetColor();
}

// FIXME: migrate to newer LLVM
/**
llvm::raw_ostream& mcld::raw_fd_ostream::reverseColor()
{
  if (!is_displayed())
    return *this;
  return llvm::raw_ostream::reverseColor();
}
**/

bool mcld::raw_fd_ostream::is_displayed() const
{
  if (NULL == m_pLDInfo)
    return llvm::raw_fd_ostream::is_displayed();

  return m_pLDInfo->options().color();
}

//===----------------------------------------------------------------------===//
//  outs(), errs(), nulls()
//===----------------------------------------------------------------------===//
mcld::raw_fd_ostream& mcld::outs() {
  // Set buffer settings to model stdout behavior.
  // Delete the file descriptor when the program exists, forcing error
  // detection. If you don't want this behavior, don't use outs().
  static mcld::raw_fd_ostream S(STDOUT_FILENO, true, NULL);
  return S;
}

mcld::raw_fd_ostream& mcld::errs() {
  // Set standard error to be unbuffered by default.
  static mcld::raw_fd_ostream S(STDERR_FILENO, false, true, NULL);
  return S;
}

void mcld::InitializeOStreams(const MCLDInfo& pLDInfo)
{
  outs().setLDInfo(pLDInfo);
  errs().setLDInfo(pLDInfo);
}

