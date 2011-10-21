//===- Directory.cpp ------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Support/Directory.h"
#include "mcld/Support/FileSystem.h"
#include <cerrno>

using namespace mcld;
using namespace mcld::sys::fs;

namespace { // anonymous

bool status_known(FileStatus f)
{
  return f.type() != StatusError;
}

bool is_symlink(FileStatus f)
{
  return f.type() == SymlinkFile;
}

} // namespace of anonymous

//==========================
// Directory
Directory::Directory()
  : m_Path(), m_FileStatus(), m_SymLinkStatus(), m_Cache(), m_Handler(NULL) {
}

Directory::Directory(const Path& pPath,
                     FileStatus st,
                     FileStatus symlink_st)
  : m_Path(pPath),
    m_FileStatus(st),
    m_SymLinkStatus(symlink_st),
    m_Cache(),
    m_Handler(NULL) {
  if (m_Path.native() == ".")
    detail::get_pwd(m_Path.native());
  m_Path.m_append_separator_if_needed();
  mcld::sys::fs::detail::open_dir(*this);
}

Directory::Directory(const Directory& pCopy)
  : m_Path(pCopy.m_Path),
    m_FileStatus(pCopy.m_FileStatus),
    m_SymLinkStatus(pCopy.m_SymLinkStatus),
    m_Cache(),
    m_Handler(NULL) {
  mcld::sys::fs::detail::open_dir(*this);
}

Directory::~Directory()
{
  detail::close_dir(*this);
}

bool Directory::isGood() const
{
  return (NULL != m_Handler);
}

Directory& Directory::operator=(const Directory& pCopy)
{
  assign(pCopy.m_Path, pCopy.m_FileStatus, pCopy.m_SymLinkStatus);
  return *this;
}

void Directory::assign(const Path& pPath,
                       FileStatus st,
                       FileStatus symlink_st)
{
  if (isGood())
    clear();

  m_Path = pPath;
  if (m_Path.native() == ".")
    detail::get_pwd(m_Path.native());
  m_Path.m_append_separator_if_needed();

  m_FileStatus = st;
  m_SymLinkStatus = symlink_st;
  detail::open_dir(*this);
}

FileStatus Directory::status() const
{
  if (!status_known(m_FileStatus))
  {
    // optimization: if the symlink status is known, and it isn't a symlink,
    // then status and symlink_status are identical so just copy the
    // symlink status to the regular status.
    if (status_known(m_SymLinkStatus)
      && !is_symlink(m_SymLinkStatus))
    {
      m_FileStatus = m_SymLinkStatus;
    }
    else detail::status(m_Path,m_FileStatus);
  }
  return m_FileStatus;

}

FileStatus Directory::symlinkStatus() const
{
  if (!status_known(m_SymLinkStatus))
     detail::symlink_status(m_Path,m_SymLinkStatus);
  return  m_SymLinkStatus;
}

Directory::iterator Directory::begin()
{
  if (!isGood() || m_Cache.empty()) {
    return end();
  }
  return iterator(this, m_Cache.begin());
}

Directory::iterator Directory::end()
{
  return iterator(0, m_Cache.begin());
}

void Directory::clear()
{
  m_Path.native().clear();
  m_FileStatus = FileStatus();
  m_SymLinkStatus = FileStatus();
  m_Cache.clear();
  detail::close_dir(*this);
}

//==========================
// DirIterator
DirIterator::DirIterator(Directory* pParent,
                         const DirIterator::DirCache::iterator& pIter)
  : m_pParent(pParent), m_Idx(pIter) {
}

DirIterator::DirIterator(const DirIterator& pCopy)
  : m_pParent(pCopy.m_pParent), m_Idx(pCopy.m_Idx) {
}

DirIterator::~DirIterator()
{
}

Path* DirIterator::path()
{
  if (m_pParent == 0) // end
    return 0;
  if (m_Idx != m_pParent->m_Cache.end())
    return *m_Idx;
  return 0;
}

const Path* DirIterator::path() const
{
  if (m_pParent == 0) // end
    return 0;
  if (m_Idx != m_pParent->m_Cache.end())
    return *m_Idx;
  return 0;
}

DirIterator& DirIterator::operator=(const DirIterator& pCopy)
{
  m_pParent = pCopy.m_pParent;
  m_Idx = pCopy.m_Idx;
}

DirIterator& DirIterator::operator++()
{
  if (0 == m_pParent) // real Directory::end() or empty directory
    return *this;

  ++m_Idx;
  if (m_Idx == m_pParent->m_Cache.end()) {
    Directory::PathCache::iterator it(detail::bring_one_into_cache(*this));
    if (it == m_pParent->m_Cache.end()) {
      m_Idx = m_pParent->m_Cache.begin();
      m_pParent = 0;
      return *this;
    }
    m_Idx = it;
  }
  return *this;
}

DirIterator DirIterator::operator++(int)
{
  DirIterator tmp(*this);
  if (0 == m_pParent)
    return tmp;

  ++m_Idx;
  if (m_Idx == m_pParent->m_Cache.end()) {
    Directory::PathCache::iterator it(detail::bring_one_into_cache(*this));
    if (it == m_pParent->m_Cache.end()) {
      m_Idx = m_pParent->m_Cache.begin();
      m_pParent = 0;
      return tmp;
    }
    m_Idx = it;
  }
  return tmp;
}

bool DirIterator::operator==(const DirIterator& y) const
{
  if (m_pParent != y.m_pParent)
    return false;
  if (0 == m_pParent)
    return true;
  const Path* x_path = path();
  const Path* y_path = y.path();
  if (0 == x_path && 0 == y_path)
    return true;
  if (0 == x_path || 0 == y_path)
    return false;
  return (*x_path == *y_path);
}

bool DirIterator::operator!=(const DirIterator& y) const
{
  return !this->operator==(y);
}

