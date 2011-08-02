/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef DIRECTORY_H
#define DIRECTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/FileSystem.h>

namespace mcld {
namespace sys {
namespace fs {

/** \class Directory
 *  \brief A Directory object stores a Path object, a FileStatus object for
 *   non-symbolic link status, and a FileStatus object for symbolic link
 *   status. The FileStatus objects act as value caches.
 */
class Directory
{
public:
  Directory() {}
  explicit Directory(const Path& pPath,
                     FileStatus st = FileStatus(),
                     FileStatus symlink_st = FileStatus())
    : m_Path(pPath), m_FileStatus(st), m_SymLinkStatus(symlink_st)
    {}

  virtual ~Directory() {}

  Directory& operator=(const Directory& pCopy);

  void assign(const Path& pPath, 
              FileStatus st = FileStatus(),
              FileStatus symlink_st = FileStatus());

  const Path& path() const;
  FileStatus status() const;
  FileStatus symlinkStatus() const;
protected:
  Path m_Path;
  mutable FileStatus m_FileStatus;
  mutable FileStatus m_SymLinkStatus;
};


/** \class DirIterator
 *  \brief A DirIterator object can traverse all entries in a Directory
 */
class DirIterator
{
public:
  DirIterator();
  DirIterator(const DirIterator& pCopy);
  DirIterator(const Directory& pDir);
  ~DirIterator();

};

} // namespace of fs
} // namespace of sys
} // namespace of mcld

#endif

