//===- header.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_GOT_H
#define MCLD_ARM_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/GOT.h>

namespace mcld
{

/** \class ARMGOT
 *  \brief ARM Global Offset Table.
 */
class ARMGOT : public GOT
{
public:
  ARMGOT();
  ~ARMGOT();
};

} // namespace of mcld

#endif

