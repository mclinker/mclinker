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

  bool hasDefaultBitcode() const;
  MCLDFile* defaultBitcode() const;
  void setDefaultBitcode(MCLDFile &pLDFile);

  bool hasDefaultLDScript() const;
  const char* defaultLDScript() const;
  void setDefaultLDScript(const std::string& pFilename);

  const sys::fs::Path& sysroot() const {
    return m_Sysroot;
  }
  void setSysroot(const sys::fs::Path &pPath);
  
private:
  MCLDFile* m_pDefaultBitcode;
  std::string m_DefaultLDScript;
  sys::fs::Path m_Sysroot;
};

} // namespace of mcld

#endif

