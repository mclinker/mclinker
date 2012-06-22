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
  return ::open(pPath.native().c_str(), pOFlag, pPerm);
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

