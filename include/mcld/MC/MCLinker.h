//===- MCLinker.h ---------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LINKER_H
#define MCLD_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <list>
#include "mcld/MC/MCLDCommand.h"

namespace mcld
{

  class MCLDCommand;
  class MCLDFile;
  class MCLDLayout;

  class TargetLDBackend;
  class MCLDInfo;

  /** \class MCLinker
   *  \brief MCLinker provides a pass to link object files.
   *
   *  \see
   */
  class MCLinker {
  public:
    explicit MCLinker(TargetLDBackend&, MCLDInfo&);
    ~MCLinker();

    // FIXME: see #80
    void addCommand( MCLDCommand& pCommand );
    // FIXME: see #80
    void setLayout( MCLDLayout& pLayout );
    void addLdFile( MCLDFile& pLDFile );

    void relocate();

  private:
    typedef std::list<MCLDCommand> CommandListTy;
    typedef std::list<MCLDFile> LDFileListTy;

    CommandListTy m_CommandList;
    LDFileListTy m_LDFileList;

    TargetLDBackend& m_pBackend;
    MCLDInfo& m_pInfo;
  };

} // namespace of BOLD

#endif

