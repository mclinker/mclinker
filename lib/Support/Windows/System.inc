//===- System.inc ---------------------------------------------------------===//
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
#include <fcntl.h>

#include <windows.h>

namespace mcld{
namespace sys{

std::string getDefaultTargetTriple()
{
  return MCLD_DEFAULT_TARGET_TRIPLE;
}

int GetPageSize()
{
  static int _pagesize = 0;
  if (! _pagesize) {
    SYSTEM_INFO sysinfo;
    GetSystemInfo (&sysinfo);
    _pagesize = sysinfo.dwPageSize;
  }
  return _pagesize;
}

} // namespace of sys
} // namespace of mcld

