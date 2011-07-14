/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCLDFILE_H
#define MCLDFILE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDContext.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <string>


namespace mcld
{
class MCLDContext;

/** \class MCLDFile
 *  \brief MCLDFile represents the file being linked or produced.
 *
 *  \see llvm::sys::Path
 */
class MCLDFile : public llvm::sys::Path
{
public:
  MCLDFile();
  ~MCLDFile();

  bool isRecognized() const;

private:
  std::string m_AbsFilePath;
  std::string m_ArgName;
  MCLDContext *m_pContext;
};

} // namespace of mcld

#endif

