//===- FileSystem.inc -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <string>

namespace mcld{
namespace sys{
namespace fs{
namespace detail{

std::string static_library_extension = ".lib";
std::string shared_library_extension = ".dll";
std::string executable_extension = ".exe";
std::string relocatable_extension = ".obj";
std::string assembly_extension = ".s";
std::string bitcode_extension = ".bc";

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
  off_t curr_pos = ::lseek(pFD, 0, SEEK_CUR);
  if (::lseek (pFD, pOffset, SEEK_SET) != pOffset)
    return -1;
  ssize_t retval = ::read(pFD, pBuf, pCount);
  if (::lseek(pFD, curr_pos, SEEK_SET) != curr_pos)
    return -1;
  return retval;
}

ssize_t pwrite(int pFD, const void* pBuf, size_t pCount, size_t pOffset)
{
  off_t curr_pos = ::lseek(pFD, 0, SEEK_CUR);
  if (::lseek(pFD, pOffset, SEEK_SET) != pOffset)
    return -1;
  ssize_t retval = ::write(pFD, pBuf, pCount);
  if (::lseek(pFD, curr_pos, SEEK_SET) != curr_pos)
    return -1;
  return retval; 
}

int ftruncate(int pFD, size_t pLength)
{
  return ::ftruncate(pFD, pLength);
}

} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

