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
#include <mcld/MC/MCRegionFragment.h>

namespace mcld
{

/** \class CIE
 *  \brief Common Information Entry.
 *  The CIE structure refers to LSB Core Spec 4.1, chap.10.6. Exception Frames.
 */
class CIE
{
public:
  explicit CIE(const MCRegionFragment& pFrag, uint8_t pFDEEncode);
  ~CIE();

  // ----- observers ----- //
  /// getFDEEncoding - get the FDE encoding from Augmentation Data if 'R'
  /// is present in Augmentaion String
  uint8_t getFDEEncode() const
  { return m_FDEEncoding; }

  const MCRegionFragment& getRegionFrag() const
  { return m_Fragment; }

  size_t size() const
  { return m_Fragment.getRegion().size(); }

private:
  const MCRegionFragment& m_Fragment;
  uint8_t m_FDEEncoding;
};

} // namespace of mcld

#endif

