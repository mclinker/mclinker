//===- X86GOTPLT.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_X86_GOTPLT_H
#define MCLD_X86_GOTPLT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/DenseMap.h>

#include <mcld/Target/GOT.h>

namespace mcld {

class X86PLT;
class LDSection;

const unsigned int X86GOTPLT0Num = 3;

/** \class X86GOTPLT
 *  \brief X86 .got.plt section.
 */
class X86GOTPLT : public GOT
{
public:
  X86GOTPLT(LDSection &pSection);

  ~X86GOTPLT();

  // hasGOT1 - return if this section has any GOT1 entry
  bool hasGOT1() const;

  void applyGOT0(uint64_t pAddress);

  void applyAllGOTPLT(const X86PLT& pPLT);
};

} // namespace of mcld

#endif

