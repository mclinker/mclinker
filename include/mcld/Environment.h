//===- Environment.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ENVIRONMENT_H
#define MCLD_ENVIRONMENT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld {

class LinkerConfig;

void Initialize(const LinkerConfig& pConfig);

void Finalize();

} // namespace of mcld

#endif
