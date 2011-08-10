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
#include <mcld/MC/MCLDInputTree.h>

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
  explicit MCLDInfo();
  virtual ~MCLDInfo();

  GeneralOptions& options() {
    return m_Options;
  }

  const GeneralOptions& options() const {
    return m_Options;
  }

  InputTree& inputs() {
    return m_InputTree;
  }

  const InputTree& inputs() const {
    return m_InputTree;
  }

private:
  GeneralOptions m_Options;
  InputTree m_InputTree;
  // MCLDInfo will have a Output containing global symble table
  //Output m_Output;
};

} // namespace of mcld

#endif

