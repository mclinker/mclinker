/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   csmon7507 <csmon7507@gmail.com>                                         *
 ****************************************************************************/
#ifndef MCLDINFO_H
#define MCLDINFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <mcld/MC/MCLDOptions.h>
#include <mcld/Support/FileSystem.h>

namespace mcld
{
class MCLDFile;
/** \class MCLDInfo
 *  \brief MCLDInfo is composed of argumments of MCLDDriver.
 *
 *  \see
 *  \author csmon7507 <csmon7507@gmail.com>
 */
class MCLDInfo
{
public:
  MCLDInfo();
  ~MCLDInfo();

  GeneralOptions* options() {
    return &m_Options;
  }

  const GeneralOptions* options() const {
    return &m_Options;
  }

private:
  GeneralOptions m_Options;
};

} // namespace of mcld

#endif

