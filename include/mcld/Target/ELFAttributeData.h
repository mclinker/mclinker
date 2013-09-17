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

#include <string>

namespace mcld {

class Input;

/** \class ELFAttributeData
 *  \brief ELFAttributeData handles data in vendor attribute subsection.
 */
class ELFAttributeData
{
public:
  ELFAttributeData(const char* pVendor) : m_Vendor(pVendor) { }

  virtual ~ELFAttributeData() { }

public:
  inline const std::string &getVendorName() const { return m_Vendor; }

private:
  const std::string m_Vendor;
};

} // namespace of mcld

#endif
