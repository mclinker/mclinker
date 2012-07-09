//===- FileSystem.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This file declares the mcld::sys::fs:: namespace. It follows TR2/boost
// filesystem (v3), but modified to remove exception handling and the
// path class.
//===----------------------------------------------------------------------===//

#ifndef MCLD_FILE_SYSTEM_H
#define MCLD_FILE_SYSTEM_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/Support/PathCache.h"
#include <mcld/Config/Config.h>
#include <string>
#include <iosfwd>
#include <locale>

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
  StatusKnown,
  IsSymLink
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

  void setType(FileType v)   { m_Value = v; }
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

class Path;
class DirIterator;
class Directory;

bool exists(const Path &pPath);
bool is_directory(const Path &pPath);

inline static bool exists(FileStatus f) {
  return (f.type() != StatusError)&&(f.type() != FileNotFound);
}

inline static bool is_directory(FileStatus f) {
  return f.type() == mcld::sys::fs::DirectoryFile;
}

namespace detail
{

extern std::string static_library_extension;
extern std::string shared_library_extension;
extern std::string executable_extension;
extern std::string relocatable_extension;
extern std::string assembly_extension;
extern std::string bitcode_extension;

size_t canonicalize(std::string& pPathName);
bool not_found_error(int perrno);
void status(const Path& p, FileStatus& pFileStatus);
void symlink_status(const Path& p, FileStatus& pFileStatus);
mcld::sys::fs::PathCache::entry_type* bring_one_into_cache(DirIterator& pIter);
void open_dir(Directory& pDir);
void close_dir(Directory& pDir);
void get_pwd(std::string& pPWD);

int open(const Path& pPath, int pOFlag);
int open(const Path& pPath, int pOFlag, int pPermission);
ssize_t pread(int pFD, void* pBuf, size_t pCount, size_t pOffset);
ssize_t pwrite(int pFD, const void* pBuf, size_t pCount, size_t pOffset);
int ftruncate(int pFD, size_t pLength);

} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

#endif

