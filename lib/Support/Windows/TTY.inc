//===- TTY.inc ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <io.h>

namespace mcld {
namespace sys {
namespace tty {

int isatty(int pFD)
{
  return ::_isatty(pFD);
}

} // namespace of tty
} // namespace of sys
} // namespace of mcld
