/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                                    *
 ****************************************************************************/
#ifndef MCLD_SCRIPT_READER_H
#define MCLD_SCRIPT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCLDScriptReader
 *
 *  \author Jush Lu <jush.msn@gmail.com>
 */
class MCLDScriptReader
{
public:
  MCLDScriptReader( const MCLDInfo& LDInfo );

  MCLDCommand* createCommand();
  MCLDLayout* createLayout();
private:
  bool hasCommand() const;
private:
  const MCLDInfo& m_LDInfo;
};

} // namespace of BOLD

#endif

