//===- FileSystem.inc -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <string>
#include <io.h>
#include <fcntl.h>
#include <cstdlib>
#include <Windows.h>
#include <limits.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/Directory.h>

#ifndef STDIN_FILENO
# define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
# define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif

namespace mcld{
namespace sys{
namespace fs{
namespace detail{

// FIXME: the extension depends on target machine, not host machine.
Path::StringType static_library_extension = ".a";
Path::StringType shared_library_extension = ".so";
Path::StringType executable_extension     = ".exe";
Path::StringType relocatable_extension    = ".o";
Path::StringType assembly_extension       = ".s";
Path::StringType bitcode_extension        = ".bc";

void open_dir(Directory& pDir)
{
  fs::Path file_filter(pDir.path());
  file_filter.append("*");

  WIN32_FIND_DATA FindFileData;
  HANDLE hFile = FindFirstFile(file_filter.c_str(), &FindFileData);
  pDir.m_Handler = reinterpret_cast<intptr_t>(hFile);

  if (INVALID_HANDLE_VALUE == hFile) {
    // set cache is full, then Directory::begin() can return end().
    pDir.m_CacheFull = true;
    return;
  }

  // find a new directory and file
  bool exist = false;
  std::string path(FindFileData.cFileName);
  fs::PathCache::entry_type* entry = pDir.m_Cache.insert(path, exist);
  if (!exist)
    entry->setValue(path);
}

void close_dir(Directory& pDir)
{
  if (pDir.m_Handler)
    FindClose(reinterpret_cast<HANDLE>(pDir.m_Handler));
  pDir.m_Handler = 0;
}

int open(const Path& pPath, int pOFlag)
{
  return ::_open(pPath.native().c_str(), pOFlag);
}

int open(const Path& pPath, int pOFlag, int pPerm)
{
  int perm = 0;
  if (pPerm & FileHandle::ReadOwner ||
      pPerm & FileHandle::ReadGroup ||
      pPerm & FileHandle::ReadOther)
    perm |= _S_IREAD;

  if (pPerm & FileHandle::WriteOwner ||
      pPerm & FileHandle::WriteGroup ||
      pPerm & FileHandle::WriteOther)
    perm |= _S_IWRITE;

  return ::_open(pPath.native().c_str(), pOFlag, perm);
}

ssize_t pread(int pFD, void* pBuf, size_t pCount, size_t pOffset)
{
  off_t curr_pos = ::_lseek(pFD, 0, SEEK_CUR);
  if (::_lseek (pFD, pOffset, SEEK_SET) != pOffset)
    return -1;
  ssize_t retval = ::_read(pFD, pBuf, pCount);
  if (::_lseek(pFD, curr_pos, SEEK_SET) != curr_pos)
    return -1;
  return retval;
}

ssize_t pwrite(int pFD, const void* pBuf, size_t pCount, size_t pOffset)
{
  off_t curr_pos = ::_lseek(pFD, 0, SEEK_CUR);
  if (::_lseek(pFD, pOffset, SEEK_SET) != pOffset)
    return -1;
  ssize_t retval = ::_write(pFD, pBuf, pCount);
  if (::_lseek(pFD, curr_pos, SEEK_SET) != curr_pos)
    return -1;
  return retval; 
}

int ftruncate(int pFD, size_t pLength)
{
  return ::_chsize(pFD, pLength);
}

void get_pwd(Path& pPWD)
{
  char* pwd = (char*)malloc(PATH_MAX);
  pPWD.assign(_getcwd(pwd, PATH_MAX));
  free(pwd);
}

} // namespace of detail
} // namespace of fs
} // namespace of sys

//===----------------------------------------------------------------------===//
// FileHandle
//===----------------------------------------------------------------------===//
bool FileHandle::mmap(void*& pMemBuffer, size_t pStartOffset, size_t pLength)
{
  // FIXME: This implementation reduces mmap to read. Use Windows APIs.
  pMemBuffer = (void*)::malloc(pLength);
  return read(pMemBuffer, pStartOffset, pLength);
}

bool FileHandle::munmap(void* pMemBuffer, size_t pLength)
{
  // FIXME: This implementation reduces mmap to read. Use Windows APIs.
  free(pMemBuffer);
  return true;
}

} // namespace of mcld

