//===- GNUInfo.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_GNU_INFO_H
#define MCLD_TARGET_GNU_INFO_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld {

class GNUInfo
{
public:
  /// The return value of machine() it the same as e_machine in the ELF header
  virtual uint32_t machine() const = 0;
};

} // namespace of mcld

#endif

