/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#ifndef MCLD_LINKER_H
#define MCLD_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

  using namespace llvm;

  class MCLDCommand;

  /** \class MCLinker
   *  \brief MCLinker provides a pass to link object files.
   *
   *  \see
   *  \author Jush Lu <jush.msn@gmail.com>
   */
  class MCLinker {
  public:
    explicit MCLinker( MCLDBackend& pBackend );
    ~MCLinker();
  public:
    // FIXME: see #80
    void addCommand( MCLDCommand& pCommand );
    // FIXME: see #80
    void setLayout( MCLDLayout& pLayout );
    void addLdFile( MCLDFile& pLDFile );
  
  private:
    typedef iplist<MCLDCommand> CommandListTy;
    typedef iplist<MCLDFile> LDFileListTy;
  private:
    CommandListTy m_CommandList;
    LDFileListTy m_LDFileList;
  };

} // namespace of BOLD

#endif

