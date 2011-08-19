/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   CSMON <chun-hung.lu@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/Directory.h>
#include <mcld/Support/FileSystem.h>
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
  : m_Path(), m_FileStatus(), m_SymLinkStatus(), m_Cache(), m_Handler(0) {
}

Directory::Directory(const Path& pPath,
                     FileStatus st,
                     FileStatus symlink_st)
  : m_Path(pPath),
    m_FileStatus(st),
    m_SymLinkStatus(symlink_st),
    m_Cache() {
  mcld::sys::fs::detail::open_dir(*this);
}

Directory::Directory(const Directory& pCopy)
  : m_Path(pCopy.m_Path),
    m_FileStatus(pCopy.m_FileStatus),
    m_SymLinkStatus(pCopy.m_SymLinkStatus),
    m_Cache() {
  mcld::sys::fs::detail::open_dir(*this);
}

Directory::~Directory()
{
  detail::close_dir(*this);
  // m_Cache erase itself in its destructor
}

Directory& Directory::operator=(const Directory& pCopy)
{
  if (!m_Cache.empty())
    detail::close_dir(*this);
  m_Path = pCopy.m_Path;
  m_FileStatus = pCopy.m_FileStatus;
  m_SymLinkStatus = pCopy.m_SymLinkStatus;
  m_Cache.clear();
  detail::open_dir(*this);
}

void Directory::assign(const Path& pPath,
                       FileStatus st,
                       FileStatus symlink_st)
{
  if (!m_Cache.empty())
    detail::close_dir(*this);
  m_Path = pPath;
  m_FileStatus = st;
  m_SymLinkStatus = symlink_st;
  m_Cache.clear();
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
  return iterator(*this, m_Cache.begin());
}

Directory::iterator Directory::end()
{
  return iterator(*this, m_Cache.end());
}

//==========================
// DirIterator
DirIterator::DirIterator(Directory& pDirectory, const DirIterator::DirCache::iterator& pIter)
  : m_pParent(&pDirectory), m_pPath(0) {
  m_pIdx = new Directory::PathCache::iterator(pIter);
}

DirIterator::DirIterator()
  : m_pParent(0), m_pIdx(0), m_pPath(0) {
}

DirIterator::DirIterator(const DirIterator& pCopy)
  : m_pParent(pCopy.m_pParent), m_pPath(pCopy.m_pPath) {
  m_pIdx = new Directory::PathCache::iterator(*pCopy.m_pIdx);
}

DirIterator::~DirIterator()
{
  delete m_pIdx;
}

DirIterator& DirIterator::operator=(const DirIterator& pCopy)
{
  m_pParent = pCopy.m_pParent;
  new(m_pIdx) Directory::PathCache::iterator(*pCopy.m_pIdx);
  m_pPath = pCopy.m_pPath;
}

DirIterator& DirIterator::operator++()
{
  if (0 == m_pIdx)
    return *this;
  if (*m_pIdx == m_pParent->m_Cache.end()) {
    // maybe reach the end of directory or just reach the end of cache.
    detail::directory_iterator_increment(*this);
  }
  else {
    ++(*m_pIdx);
    m_pPath = &(*m_pIdx)->getValue();
  }
  return *this;
}

DirIterator DirIterator::operator++(int)
{
  DirIterator tmp(*this);
  if (0 == m_pIdx)
    return tmp;

  if (*m_pIdx == m_pParent->m_Cache.end()) {
    // maybe reach the end of directory or just reach the end of cache.
    detail::directory_iterator_increment(*this);
  }
  else {
    ++(*m_pIdx);
    m_pPath = &(*m_pIdx)->getValue();
  }
  return tmp;
}

bool DirIterator::operator==(const DirIterator& y) const
{
  if (m_pPath == y.m_pPath)
    return true;
  if (m_pPath->generic_string() == y.m_pPath->generic_string())
    return true;
  return false;
}

bool DirIterator::operator!=(const DirIterator& y) const
{
  return !this->operator==(y);
}

