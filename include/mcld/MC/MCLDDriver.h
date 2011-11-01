//===- MCLDDriver.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// MCLDDriver plays the same role as GNU collect2 to prepare all implicit
// parameters for MCLinker.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LDDRIVER_H
#define MCLD_LDDRIVER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

//FIXME: The implementation of MCLDDriver is currently
//       in SectLinker::doInitialization. those code should
//       be migrated here.
namespace mcld {
class MCLDDriver {


};

} // end namespace mcld
#endif
