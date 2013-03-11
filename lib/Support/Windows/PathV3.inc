//===- PathV3.inc ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/Path.h>

namespace mcld{
namespace sys{
namespace fs{

//===----------------------------------------------------------------------===//
// mcld::sys::fs::Path
//===----------------------------------------------------------------------===//
std::string Path::string() const
{
  std::string result;
  if (!m_PathName.empty()) {
    // TODO: convert
  }
  return result;
}

std::wstring Path::wstring() const
{
  return m_PathName;
}

//===----------------------------------------------------------------------===//
// mcld::sys::fs::detail
//===----------------------------------------------------------------------===//
namespace detail{

} // namespace of detail
} // namespace of fs
} // namespace of sys
} // namespace of mcld

