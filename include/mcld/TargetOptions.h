//===- TargetOptions.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_OPTIONS_H
#define MCLD_TARGET_OPTIONS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld {

/** \class TargetOptions
 *  \brief TargetOptions collects the options that dependent on a target
 *  backend.
 */
class TargetOptions
{
public:
  TargetOptions();
  ~TargetOptions();
};

} // namespace of mcld

#endif

