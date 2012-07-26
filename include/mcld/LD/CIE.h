//===- CIE.h --------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_COMMON_INFORMATION_ENTRY_H
#define MCLD_COMMON_INFORMATION_ENTRY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/DataTypes.h>
#include <mcld/LD/RegionFragment.h>

namespace mcld
{

/** \class CIE
 *  \brief Common Information Entry.
 *  The CIE structure refers to LSB Core Spec 4.1, chap.10.6. Exception Frames.
 */
class CIE : public RegionFragment
{
public:
  explicit CIE(MemoryRegion& pRegion, uint8_t pFDEEncode);
  ~CIE();

  // ----- observers ----- //
  /// getFDEEncoding - get the FDE encoding from Augmentation Data if 'R'
  /// is present in Augmentaion String
  uint8_t getFDEEncode() const
  { return m_FDEEncoding; }

private:
  uint8_t m_FDEEncoding;
};

} // namespace of mcld

#endif

