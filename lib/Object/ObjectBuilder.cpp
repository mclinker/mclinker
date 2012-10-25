//===- ObjectBuilder.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Object/ObjectBuilder.h>
#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Object/SectionMap.h>
#include <mcld/LD/SectionRules.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Fragment/AlignFragment.h>
#include <mcld/Fragment/NullFragment.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// ObjectBuilder
//===----------------------------------------------------------------------===//
ObjectBuilder::ObjectBuilder(const LinkerConfig& pConfig, Module& pTheModule)
  : m_Config(pConfig), m_Module(pTheModule), m_pRules(NULL) {
}

/// CreateSection - create output section.
LDSection* ObjectBuilder::CreateSection(const std::string& pName,
                                        LDFileFormat::Kind pKind,
                                        uint32_t pType,
                                        uint32_t pFlag,
                                        uint32_t pAlign)
{
  LDSection* result = NULL;
  if (NULL == m_pRules)
    result = LDSection::Create(pName, pKind, pType, pFlag, pAlign);
  else {
    result = m_pRules->getMatchedSection(pName);
    if (NULL != result)
      return result;
    else {
      const SectionMap::NamePair& new_name =
                                   m_Config.scripts().sectionMap().find(pName);
      if (!new_name.isNull()) {
        // Find a rule in the link script. Add the rule in SectionRules.
        result = LDSection::Create(new_name.to, pKind, pType, pFlag, pAlign);
        m_pRules->append(new_name.from, *result);
      }
      else {
        // Not found a rule in the link script.
        // Sections with identical name will be merged togather, therefore, add
        // a rule in SectionRules to merge sections with identical name.
        result = LDSection::Create(pName, pKind, pType, pFlag, pAlign);
        m_pRules->append(pName, *result);
      }
    }
  }

  SectionData* data = SectionData::Create(*result);
  result->setSectionData(data);
  m_Module.getSectionTable().push_back(result);
  return result;
}

bool ObjectBuilder::MergeSection(LDSection& pOutput, LDSection& pInput)
{
  // Step 1. Get the last offset in the output section.
  uint64_t offset = 0;
  if (!pOutput.getSectionData()->empty()) {
    Fragment* last = &(pOutput.getSectionData()->back());
    offset = last->getOffset() + last->size();
  }

  // Step 2. Insert and update alignment
  AlignFragment* align_frag = NULL;
  bool should_align = (pInput.align() > pOutput.align()) && (pInput.align() > 1);
  if (!pOutput.getSectionData()->empty() && should_align) {
    // insert AlignFragment into SectionData first if alignment constraint is
    // not trivial and pFrag is not the first fragment of the section data.
    align_frag = new AlignFragment(pInput.align(), // alignment
                                   0x0, // the filled value
                                   1u,  // the size of filled value
                                   pInput.align() - 1, // max bytes to emit
                                   pOutput.getSectionData());

    align_frag->setOffset(offset);
    offset += align_frag->size();
  }

  if (should_align)
    pOutput.setAlign(pInput.align());

  // Step 3. Move fragments from pInput section to pOutput section.
  SectionData::iterator frag, fragEnd = pInput.getSectionData()->end();
  for (frag = pInput.getSectionData()->begin(); frag != fragEnd; ++frag) {
    frag->setParent(pOutput.getSectionData());
    frag->setOffset(offset);
    offset += frag->size();
  }

  pOutput.getSectionData()->getFragmentList().splice(
                                   pOutput.getSectionData()->end(),
                                   pInput.getSectionData()->getFragmentList());

  // Step 4. Add NullFragment as the end of the pOutput section.
  NullFragment* null = new NullFragment(pOutput.getSectionData());
  null->setOffset(offset);

  // Step 5. set up output section size.
  pOutput.setSize(offset);
  return true;
}

