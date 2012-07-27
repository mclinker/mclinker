//===- FDE.h --------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_FRAME_DESCRIPTION_ENTRY_H
#define MCLD_LD_FRAME_DESCRIPTION_ENTRY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/DataTypes.h>
#include <mcld/LD/CIE.h>
#include <mcld/LD/RegionFragment.h>

namespace mcld
{

/** \class FDE
 *  \brief Frame Description Entry
 *  The FDE structure refers to LSB Core Spec 4.1, chap.10.6. Exception Frames.
 */
class FDE : public RegionFragment
{
public:
  typedef uint32_t Offset;

public:
  FDE(MemoryRegion& pRegion, const CIE& pCIE, Offset pPCBeginOffset);
  ~FDE();

  /// ----- observers ------ ///
  /// getCIE - the CIE corresponding to this FDE
  const CIE& getCIE() const
  { return m_CIE; }

  /// getPCBeginOffset - the offset to the FDE of the PC Begin field
  Offset getPCBeginOffset() const
  { return m_PCBeginOffset; }

private:
  const CIE& m_CIE;
  Offset m_PCBeginOffset;
};

} // namespace of mcld

#endif

