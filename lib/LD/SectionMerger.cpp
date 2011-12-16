//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <cassert>
#include <mcld/LD/SectionMerger.h>
#include <mcld/LD/LDSection.h>

using namespace mcld;

//==========================
// SectionMerger

SectionMerger::SectionMerger(MCLinker& pLinker)
: m_Linker(pLinker),
  m_SectionMap(pLinker.getSectionMap()),
  m_SectionList(pLinker.getSectionList())
{
  // initialize the map from "name" to associated "section data"
  for (MCLinker::section_list_iterator it = m_Linker.section_list_begin();
       it != m_Linker.section_list_end(); ++it) {
    assert(NULL != (*it));
    const LDSection& section =
      static_cast<const LDSection&>((*it)->getSection());
    m_SectionDataMap[section.name()] = *it;
  }
}

SectionMerger::~SectionMerger()
{
}

llvm::MCSectionData& SectionMerger::getOutputSectionData(const std::string& pInputSectName)
{
  llvm::MCSectionData* sectionData = m_SectionDataMap.find(
    m_SectionMap.getOutputSectionName(pInputSectName))->second;
  assert(NULL != sectionData);
  return *sectionData;
}
