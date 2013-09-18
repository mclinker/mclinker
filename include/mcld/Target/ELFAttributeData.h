//===- ELFAttributeData.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_ATTRIBUTE_DATA_H
#define MCLD_ELF_ATTRIBUTE_DATA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <stdint.h>
#include <string>
#include <utility>

namespace mcld {

class ELFAttributeValue;
class Input;

/** \class ELFAttributeData
 *  \brief ELFAttributeData handles data in vendor attribute subsection.
 */
class ELFAttributeData
{
public:
  typedef uint32_t TagType;

  // Generic attribute tags shared between all vendors
  enum {
    Tag_NULL          = 0,
    Tag_File          = 1,
    Tag_Section       = 2,
    Tag_Symbol        = 3,
  };

public:
  ELFAttributeData(const char* pVendor) : m_Vendor(pVendor) { }

  virtual ~ELFAttributeData() { }

public:
  inline const std::string &getVendorName() const { return m_Vendor; }

  /// getOrCreateAttributeValue - obtain attribute value for given tag and
  /// create if it does not exist.
  virtual ELFAttributeValue &getOrCreateAttributeValue(TagType pTag) = 0;

  /// merge - implement logics to merge input attribute to the output.
  virtual bool merge(const Input &pInput, TagType pTag,
                     const ELFAttributeValue& pInAttr) = 0;

public:
  /// ReadTag - read an attribute tag from input buffer.
  ///
  /// If the read succeeds, pBuf moves to the new position just pass the end of
  /// the tag in the buffer and pBufSize decreases the size of tag in the
  /// buffer. Otherwise, this function will return false and change nothing
  /// except leaving undefined value in pTag.
  static bool ReadTag(TagType& pTag, const char* &pBuf, size_t &pBufSize);

  /// ReadValue - read an attribute value from input buffer
  ///
  /// Similar with ReadTag() while this reads attribute value from the input
  /// buffer. Note that the value type of the attribute must be properly set in
  /// pValue prior the call.
  static bool ReadValue(ELFAttributeValue& pValue, const char* &pBuf,
                        size_t &pBufSize);

private:
  const std::string m_Vendor;
};

} // namespace of mcld

#endif
