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

namespace mcld{
namespace sys{

char *strerror(int errnum)
{
  return ::strerror(errnum);
}

} // namespace of sys
} // namespace of mcld

