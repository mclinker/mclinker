/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDDirectory.h>

using namespace mcld;

//==========================
// MCLDDirectory
MCLDDirectory::MCLDDirectory()
  : m_bInSysroot(false) {
}

MCLDDirectory::MCLDDirectory(const std::string &pName)
  : Directory(sys::fs::Path(pName)), m_Name(pName) {
  m_bInSysroot = ('=' == m_Name.c_str()[0]);
}

MCLDDirectory::~MCLDDirectory()
{
}

bool MCLDDirectory::isInSysroot() const
{
  return m_bInSysroot;
}

void MCLDDirectory::setSysroot(const sys::fs::Path& pPath)
{
  if (m_bInSysroot) {
    std::string::const_iterator sB = pPath.native().begin();
    std::string::const_iterator sE = pPath.native().end();
    ++sB; // skip '='
    Directory::m_Path = pPath;
    Directory::m_Path.append<std::string::const_iterator>(sB, sE);
  }
}

const sys::fs::Path &MCLDDirectory::path() const
{
  return Directory::m_Path;
}

llvm::StringRef MCLDDirectory::name() const
{
  return m_Name;
}
