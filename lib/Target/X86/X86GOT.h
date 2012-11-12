//===- X86GOT.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_X86_GOT_H
#define MCLD_TARGET_X86_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/GOT.h>

namespace mcld {

class LDSection;
class SectionData;

/** \class X86GOT
 *  \brief X86 Global Offset Table.
 */

class X86GOT : public GOT
{
public:
  X86GOT(LDSection& pSection);

  ~X86GOT();
};

} // namespace of mcld

#endif

