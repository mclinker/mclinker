//===- TTY.inc ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <termios.h>

namespace mcld {
namespace sys {
namespace tty {

int isatty(int pFD)
{
  return ::isatty(pFD);
}

} // namespace of tty
} // namespace of sys
} // namespace of mcld
