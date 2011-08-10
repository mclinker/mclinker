/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
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
  class RelocData;

  /** \class MCLinker
   *  \brief MCLinker provides a pass to link object files.
   *
   *  \see
   *  \author Jush Lu <jush.msn@gmail.com>
   */
  class MCLinker {
  public:
    explicit MCLinker(TargetLDBackend&, MCLDInfo&);
    ~MCLinker();
  public:
    // FIXME: see #80
    void addCommand( MCLDCommand& pCommand );
    // FIXME: see #80
    void setLayout( MCLDLayout& pLayout );
    void addLdFile( MCLDFile& pLDFile );

    void relocate();
  private:
    void read_relocs();
    void scan_relocs();
    void relocate_relocs();

  private:
    typedef std::list<MCLDCommand> CommandListTy;
    typedef std::list<MCLDFile> LDFileListTy;
  private:
    CommandListTy m_CommandList;
    LDFileListTy m_LDFileList;

    TargetLDBackend& m_pBackend;
    MCLDInfo& m_pInfo;
    RelocData* m_pRelocData;
  };

} // namespace of BOLD

#endif

