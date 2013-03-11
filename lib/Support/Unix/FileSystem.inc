//===- FileSystem.inc -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <mcld/Support/FileHandle.h>

namespace mcld{
namespace sys{
namespace fs{
namespace detail{

std::string static_library_extension = ".a";
std::string shared_library_extension = ".so";
std::string executable_extension = "";
std::string relocatable_extension = ".o";
std::string assembly_extension = ".s";
std::string bitcode_extension = ".bc";

int open(const Path& pPath, int pOFlag)
{
  return ::open(pPath.native().c_str(), pOFlag);
}

int open(const Path& pPath, int pOFlag, int pPerm)
{
  mode_t perm = 0;
  if (pPerm & FileHandle::ReadOwner)
    perm |= S_IRUSR;
  if (pPerm & FileHandle::WriteOwner)
    perm |= S_IWUSR;
  if (pPerm & FileHandle::ExeOwner)
    perm |= S_IXUSR;
  if (pPerm & FileHandle::ReadGroup)
    perm |= S_IRGRP;
  if (pPerm & FileHandle::WriteGroup)
    perm |= S_IWGRP;
  if (pPerm & FileHandle::ExeGroup)
    perm |= S_IXGRP;
  if (pPerm & FileHandle::ReadOther)
    perm |= S_IROTH;
  if (pPerm & FileHandle::WriteOther)
    perm |= S_IWOTH;
  if (pPerm & FileHandle::ExeOther)
    perm |= S_IXOTH;

  return ::open(pPath.native().c_str(), pOFlag, perm);
}

ssize_t pread(int pFD, void* pBuf, size_t pCount, size_t pOffset)
{
  return ::pread(pFD, pBuf, pCount, pOffset);
}

ssize_t pwrite(int pFD, const void* pBuf, size_t pCount, size_t pOffset)
{
  return ::pwrite(pFD, pBuf, pCount, pOffset);
}

int ftruncate(int pFD, size_t pLength)
{
  return ::ftruncate(pFD, pLength);
}

} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

