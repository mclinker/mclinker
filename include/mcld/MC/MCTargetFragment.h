//===- MCTargetFragment.h - MCFragment containing MemoryRegion ------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_TARGET_FRAGMENT_H
#define MCLD_TARGET_FRAGMENT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/MC/MCAssembler.h>

namespace mcld
{

/** \class MCTargetFragment
 *  \brief MCTargetFragment is a kind of MCFragment inherited by
 *  target-depedent MCFragment.
 */
class MCTargetFragment : public llvm::MCFragment
{
protected:
  MCTargetFragment(llvm::MCFragment::FragmentType pKind,
                   llvm::MCSectionData* pSD = 0) :
                   llvm::MCFragment(pKind, pSD) {}

public:
  virtual ~MCTargetFragment() {}

  virtual size_t getSize() const = 0;

public:
  static bool classof(const MCFragment *F)
  { return F->getKind() == llvm::MCFragment::FT_Target; }

  static bool classof(const MCTargetFragment *)
  { return true; }
};

} // namespace of mcld

#endif

