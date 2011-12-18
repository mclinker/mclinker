//===- SectionFactory.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SECTION_FACTORY_H
#define MCLD_SECTION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/GCFactory.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDFileFormat.h>
#include <string>

namespace mcld
{

/** \class SectionFactory
 *  \brief provide the interface to create and delete section data for output
 */
class SectionFactory : public GCFactory<LDSection, 0>
{
public:
  /// SectionFactory - the factory of LDSection
  /// pNum is the average number of the LDSections in the system.
  SectionFactory(size_t pNum);
  ~SectionFactory();

  /// produce - produce an empty section information.
  /// This function will create an empty MCSectionData and its LDSection.
  /// @param pName - The name of the section.
  /// @param pKind - The kind of the section. Used to create default section map
  /// @param pType - sh_type in ELF.
  /// @param pFlag - is the same as sh_flags.
  LDSection* produce(const std::string& pName,
                     LDFileFormat::Kind pKind,
                     uint32_t pType,
                     uint32_t pFlag);

  /// destroy - destruct the LDSection.
  /// @oaram - the reference of the pointer to the destructed LDSection.
  ///          after the destruction, the pointer is set to zero.
  void destroy(LDSection*& pSD);

};

} // namespace of mcld

#endif

