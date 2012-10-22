//===- ObjectBuilder.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OBJECT_OBJECT_BUILDER_H
#define MCLD_OBJECT_OBJECT_BUILDER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/LDFileFormat.h>
#include <llvm/Support/DataTypes.h>

namespace mcld {

class LinkerConfig;
class Module;
class LDSection;
class SectionRules;

/** \class ObjectBuilder
 *  \brief ObjectBuilder recieve ObjectAction and build the mcld::Module.
 */
class ObjectBuilder
{
public:
  ObjectBuilder(const LinkerConfig& pConfig,
                Module& pTheModule);

//===----------------------------------------------------------------------===//
// Section Methods
//===----------------------------------------------------------------------===//
  /// ClearSectionMappingRules - Clear the mapping rules: creating sections will
  /// not add a new mapping rule.
  void ClearSectionMappingRules() {
    m_pRules = NULL;
  }

  /// SetSectionMappingRules - the name of the created section will be changed
  /// by the specified mapping rules.
  void SetSectionMappingRules(SectionRules* pRules) {
    m_pRules = pRules;
  }

  /// CreateSection - create the output LDSection.
  LDSection* CreateSection(const std::string& pName,
                           LDFileFormat::Kind pKind,
                           uint32_t pType,
                           uint32_t pFlag,
                           uint32_t pAlign = 0x0);

  /// MergeSection - merge the pInput section to the pOutput section.
  static bool MergeSection(LDSection& pOutput, LDSection& pInput);

private:
  const LinkerConfig& m_Config;
  Module& m_Module;
  SectionRules* m_pRules;
};

} // namespace of mcld

#endif

