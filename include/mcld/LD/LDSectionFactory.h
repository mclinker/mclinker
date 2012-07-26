//===- LDSectionFactory.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDSECTION_FACTORY_H
#define MCLD_LDSECTION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <string>

#include <mcld/Support/GCFactory.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDFileFormat.h>

namespace mcld
{

/** \class LDSectionFactory
 *  \brief provide the interface to create and delete section data for output
 */
class LDSectionFactory : public GCFactory<LDSection, 0>
{
public:
  /// LDSectionFactory - the factory of LDSection
  /// pNum is the average number of the LDSections in the system.
  LDSectionFactory(size_t pNum);
  ~LDSectionFactory();

  /// produce - produce an empty section information.
  /// This function will create an empty SectionData and its LDSection.
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

  /// find - find the LDSection* in factory from the given section name.
  ///        return NULL if not found.
  /// @param pName - the name of section
  LDSection* find(const std::string& pName);
};

} // namespace of mcld

#endif

