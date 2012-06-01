//===- FDE.h --------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_FRAME_DESCRIPTION_ENTRY_H
#define MCLD_FRAME_DESCRIPTION_ENTRY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/DataTypes.h>
#include <mcld/LD/CIE.h>
#include <mcld/MC/MCRegionFragment.h>

namespace mcld
{

/** \class FDE
 *  \brief Frame Description Entry
 *  The FDE structure refers to LSB Core Spec 4.1, chap.10.6. Exception Frames.
 */

class CIE;

class FDE
{
public:
  FDE(const MCRegionFragment& pFrag, const CIE& pCIE);
  ~FDE();

  /// ----- observers ------ ///
  /// length - length of the FDE structure
  uint64_t length() const;

  /// getCIE - the CIE corresponding to this FDE
  const CIE& getCIE() const;

  /// PCBegin - PC Begin
  uint64_t PCBegin() const;

  /// PCRange - PC Range
  uint64_t PCRange() const;

private:
  const MCRegionFragment& m_Fragment;
  const CIE& m_CIE;
};

} // namespace of mcld

#endif

