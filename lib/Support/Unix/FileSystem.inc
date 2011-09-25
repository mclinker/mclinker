/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
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

size_t pread(int pFD, Address pBuf, size_t pCount, off_t pOffset)
{
  return ::pread(pFD, (void*) pBuf, pCount, pOffset);
}

size_t pwrite(int pFD, const Address pBuf, size_t pCount, off_t pOffset)
{
  return ::pwrite(pFD, (const void*) pBuf, pCount, pOffset);
}

char *strerror(int errnum)
{
  return ::strerror(errnum);
}

} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

