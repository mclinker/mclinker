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

class FDE
{
public:
  FDE(MCRegionFragment& pFrag, const CIE& pCIE);
  ~FDE();

  /// ----- observers ------ ///
  /// length - value of Length feild
  uint32_t length() const;

  /// extendedLength - value of Extended Length field value
  uint64_t extendedLength() const;

  /// ciePointer - value of CIE Pointer, this presents the offset between the
  /// start of this FDE to the start of the corresponding CIE
  uint32_t ciePointer() const;

  /// PCBegin - PC Begin
  uint64_t PCBegin() const;

  /// PCRange - PC Range
  uint64_t PCRange() const;

private:
  MCRegionFragment& m_Fragment;
  const CIE& m_CIE;
};

} // namespace of mcld

#endif

