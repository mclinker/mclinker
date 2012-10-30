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
#include <mcld/LD/EhFrame.h>
#include <llvm/Support/DataTypes.h>

namespace mcld {

class LinkerConfig;
class Module;
class LDSection;
class SectionData;
class RelocData;
class Fragment;
class Relocation;

/** \class ObjectBuilder
 *  \brief ObjectBuilder recieve ObjectAction and build the mcld::Module.
 */
class ObjectBuilder
{
public:
  ObjectBuilder(const LinkerConfig& pConfig,
                Module& pTheModule);

/// @}
/// @name Section Methods
/// @{
  /// CreateSection - To create an output LDSection in mcld::Module.
  /// Link scripts and command line options define some SECTIONS commands that
  /// specify where input sections are placed into output sections. This function
  /// checks SECTIONS commands to change given name to the output section name.
  /// This function creates a new LDSection and push the created LDSection into
  /// @ref mcld::Module.
  ///
  /// To create an input LDSection in mcld::LDContext, use @ref LDSection::Create().
  ///
  /// @see SectionMap
  ///
  /// @param [in] pName The given name. Returned LDSection used the changed name
  ///                   by SectionMap.
  LDSection* CreateSection(const std::string& pInputName,
                           LDFileFormat::Kind pKind,
                           uint32_t pType,
                           uint32_t pFlag,
                           uint32_t pAlign = 0x0);

  /// CreateSectionData - To create a section data for given pSection.
  /// @param [in, out] pSection The given input or output LDSection.
  ///         pSection.getSectionData() is set to a valid section data.
  /// @return The created section data. If the pSection already has section
  ///         data, or if the pSection's type should not have a section data
  ///         (.eh_frame or relocation data), then an assertion occurs.
  SectionData* CreateSectionData(LDSection& pSection);

  /// CreateRelocData - To create a relocation data for given pSection.
  /// @param [in, out] pSection The given input or output LDSection.
  ///         pSection.getRelocData() is set to a valid relocation data.
  /// @return The created relocation data. If the pSection already has
  ///         relocation data, or if the pSection's type is not
  ///         LDFileFormat::Relocation, then an assertion occurs.
  RelocData* CreateRelocData(LDSection &pSection);

  /// CreateEhFrame - To create a eh_frame for given pSection
  /// @param [in, out] pSection The given input or output LDSection.
  ///         pSection.getEhFrame() is set to a valid eh_frame.
  /// @return The created eh_frame. If the pSection already has eh_frame data,
  ///         or if the pSection's type is not LDFileFormat::EhFrame, then
  ///         return NULL.
  EhFrame* CreateEhFrame(LDSection& pSection);

  /// MergeSection - merge the pInput section to the pOutput section.
  static bool MergeSection(LDSection& pOutput, LDSection& pInput);

/// @}
/// @name Fragment Methods
/// @{
  /// AppendFragment - To append pFrag to the given LDSection pSection.
  /// In order to keep the alignment of pFrag, This function may insert an
  /// AlignFragment before pFrag to keep align constraint.
  ///
  /// @param [in] pFrag The appended fragment.
  /// @param [in, out] pSection The LDSection being appended. If pSection does
  ///        corresponding section data, relocation data or eh_frame, then an
  ///        assertion occurs. pSection's alignment constraint is reset to
  ///        pAlignConstraint if pAlignConstraint is stricter.
  /// @param [in] pAlignConstraint The alignment constraint.
  /// @return Total size of the inserted fragments.
  uint64_t AppendFragment(Fragment& pFrag, LDSection& pSection,
                          uint32_t pAlignConstraint = 1);

  /// AppendFragment - To append pFrag to the given SectionData pSD.
  /// In order to keep the alignment of pFrag, This function inserts an
  /// AlignFragment before pFrag if pAlignConstraint is larger than 1.
  ///
  /// @note This function does not update the alignment constraint of LDSection.
  ///
  /// @param [in] pFrag The appended fragment.
  /// @param [in, out] pSD The section data being appended.
  /// @param [in] pAlignConstraint The alignment constraint.
  /// @return Total size of the inserted fragments.
  uint64_t AppendFragment(Fragment& pFrag, SectionData& pSD,
                          uint32_t pAlignConstraint = 1);

  /// AppendRelocation - To append an relocation to the given RelocData pRD.
  ///
  /// @param [in] pRelocation The appended relocation.
  /// @param [in, out] pRD The relocation data being appended.
  /// @return Total size of the inserted fragments.
  uint64_t AppendRelocation(Relocation& pRelocation, RelocData& pRD);

  /// AppendEhFrame - To append a FDE to the given EhFrame pEhFram.
  ///
  /// @param [in] pFDE The appended FDE entry.
  /// @param [in, out] pEhFrame The eh_frame being appended.
  uint64_t AppendEhFrame(EhFrame::FDE& pFDE, EhFrame& pRD);

  /// AppendEhFrame - To append a CIE to the given EhFrame pEhFram.
  ///
  /// @param [in] pCIE The appended CIE entry.
  /// @param [in, out] pEhFrame The eh_frame being appended.
  uint64_t AppendEhFrame(EhFrame::CIE& pCIE, EhFrame& pRD);

private:
  const LinkerConfig& m_Config;
  Module& m_Module;
};

} // namespace of mcld

#endif

