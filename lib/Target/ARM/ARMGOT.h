//===- ARMGOT.h -----------------------------------------------------------===//
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

#include <llvm/ADT/DenseMap.h>

#include <mcld/Target/GOT.h>

namespace mcld {

class LDSection;
class MemoryRegion;

/** \class ARMGOT
 *  \brief ARM Global Offset Table.
 *
 *  ARM GOT integrates traditional .got.plt and .got sections into one.
 *  Traditional .got.plt is placed in the front part of GOT (PLTGOT), and
 *  traditional .got is placed in the rear part of GOT (GOT).
 *
 *  ARM .got
 *            +--------------+
 *            |    GOT0      |
 *            +--------------+
 *            |    GOTPLT    |
 *            +--------------+
 *            |    GOT       |
 *            +--------------+
 *
 */
class ARMGOT : public GOT
{
public:
  ARMGOT(LDSection &pSection);

  ~ARMGOT();

  void reserveGOTPLT();

  void reserveGOT();

  GOT::Entry* consumeGOT();

  GOT::Entry* consumeGOTPLT();

  uint64_t emit(MemoryRegion& pRegion);

  void applyGOT0(uint64_t pAddress);

  void applyGOTPLT(uint64_t pPLTBase);

  bool hasGOT1() const;

private:
  struct Part {
  public:
    Part() : front(NULL), last_used(NULL) { }

  public:
    GOT::Entry* front;
    GOT::Entry* last_used;
  };

private:
  Part m_GOTPLT;
  Part m_GOT;

};

} // namespace of mcld

#endif

