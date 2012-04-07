//===- LinkerErrors.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LINKER_ERRORS_H
#define MCLD_LINKER_ERRORS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

enum LinkerError
{
  Success = 0
};

} // namespace of mcld

#endif

