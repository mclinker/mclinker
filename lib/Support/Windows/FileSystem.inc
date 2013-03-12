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
#include <mcld/Support/FileHandle.h>

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

Path::StringType static_library_extension = L".lib";
Path::StringType shared_library_extension = L".dll";
Path::StringType executable_extension     = L".exe";
Path::StringType relocatable_extension    = L".obj";
Path::StringType assembly_extension       = L".s";
Path::StringType bitcode_extension        = L".bc";

int open(const Path& pPath, int pOFlag)
{
  return ::_wopen(pPath.native().c_str(), pOFlag);
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

  return ::_wopen(pPath.native().c_str(), pOFlag, perm);
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

