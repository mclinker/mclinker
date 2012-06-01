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
  CIE(MCRegionFragment& pFrag);
  ~CIE();

  // ----- observers ----- //
  /// length - value of Length feild
  uint32_t length() const;

  /// extendedLength - value of Extended Length field value
  uint64_t extendedLength() const;

  /// id - CIE ID, this shall be 0
  uint32_t id() const;

  /// version - Version field, this shall be 1
  uint8_t version() const;

  /// augString - Augmentation String
  const char* augString() const;

  /// codeAlignFactor - Code Alignment Factor
  uint32_t codeAlignFactor() const;

  /// dataAlignFactor - Data Alignment Factor
  uint32_t dataAlignFactor() const;

  /// returnAddressReg - Return Address Register
  uint8_t returnAddressReg() const;

  /// augLength - Augmentaion Length, 0 if non
  uint32_t augLength() const;

  /// getFDEEncoding - get the FDE encoding from Augmentation Data if 'R'
  /// is present in Augmentaion String
  uint8_t getFDEEncode() const;

  /// getPerOffset - get the offset to the personality routine address if
  /// 'P' is present in Augmentaion String
  uint32_t getPerOffset() const;

  /// getPerName - get the name of the personality routine
  /// FIXME: this can only extract from the symbol name of relocation which
  /// apply to this CIE, should this function be here?
  const char* getPerName() const;

private:
  MCRegionFragment& m_Fragment;
};

} // namespace of mcld

#endif

