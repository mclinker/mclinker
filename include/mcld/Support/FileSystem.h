/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 *****************************************************************************
 * This file declares the llvm::sys::fs:: namespace. It follows TR2/boost    *
 * filesystem (v3), but modified to remove exception handling and the        *
 * path class.                                                               *
 ****************************************************************************/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <iosfwd>
#include <locale>
#include <mcld/Support/Path.h>

namespace mcld {
namespace sys {
namespace fs {

enum FileType
{
  StatusError,
  StatusUnknown = StatusError,
  FileNotFound,
  RegularFile,
  DirectoryFile,
  SymlinkFile,
  BlockFile,
  CharacterFile,
  FifoFile,
  SocketFile,
  ReparseFile,
  TypeUnknown,
};

/** \class FileStatus
 *  \brief FileStatus
 */
class FileStatus
{
public:
  FileStatus() 
    : m_Value(StatusError) {}

  explicit FileStatus(FileType v)
    : m_Value(v) {}

  void type(FileType v)   { m_Value = v; }
  FileType type() const   { return m_Value; }

private:
  FileType m_Value;
};

inline bool operator==(const FileStatus& rhs, const FileStatus& lhs) {
  return rhs.type() == lhs.type();
}

inline bool operator!=(const FileStatus& rhs, const FileStatus& lhs ) {
  return !(rhs == lhs);
}

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

protected:
  Path m_Path;
  mutable FileStatus m_FileStatus;
  mutable FileStatus m_SymLinkStatus;
};

/** \class SearchDirs
 *
 */
class SearchDirs
{
public:
  SearchDirs();
  ~SearchDirs();

  void add(const Directory& pDir);
};

} // namespace of fs
} // namespace of sys
} // namespace of mcld

#endif

