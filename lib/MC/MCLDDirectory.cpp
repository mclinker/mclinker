/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDDirectory.h>
#include <mcld/Support/FileSystem.h>

using namespace mcld;
using namespace mcld::sys::fs;

//==========================
// MCLDDirectory
MCLDDirectory::MCLDDirectory()
  : Directory(), m_Name(), m_bInSysroot(false) {
}

MCLDDirectory::MCLDDirectory(const std::string &pName)
  : Directory(), m_Name(pName) {
  Directory::m_Path.assign(pName);

  if (!Directory::m_Path.empty())
    m_bInSysroot = ('=' == Directory::m_Path.native()[0]);

  if (m_bInSysroot)
    Directory::m_Path.native().erase(Directory::m_Path.native().begin());
  detail::open_dir(*this);
}

MCLDDirectory::MCLDDirectory(llvm::StringRef pName)
  : Directory(), m_Name(pName.data(), pName.size()) {
  Directory::m_Path.assign(pName.str());

  if (!Directory::m_Path.empty())
    m_bInSysroot = ('=' == Directory::m_Path.native()[0]);

  if (m_bInSysroot)
    Directory::m_Path.native().erase(Directory::m_Path.native().begin());

  detail::open_dir(*this);
}

MCLDDirectory &MCLDDirectory::assign(llvm::StringRef pName)
{
  m_Name.assign(pName.data(), pName.size());
  Directory::m_Path.assign(pName.str());

  if (!Directory::m_Path.empty())
    m_bInSysroot = ('=' == Directory::m_Path.native()[0]);

  if (m_bInSysroot)
    Directory::m_Path.native().erase(Directory::m_Path.native().begin());

  detail::open_dir(*this);
  Directory::m_FileStatus = FileStatus();
  Directory::m_SymLinkStatus = FileStatus();
  Directory::m_Cache.clear();
  Directory::m_Handler = 0;
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
    std::string old_path = Directory::m_Path.native();
    Directory::m_Path.native() = pPath.native();
    Directory::m_Path.m_append_separator_if_needed();
    Directory::m_Path.native() += old_path;
    detail::canonicalize(Directory::m_Path.native());
  }
}

