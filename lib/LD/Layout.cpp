//===- Layout.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/Layout.h>

#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/ADT/SizeTraits.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDSection.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/TargetLDBackend.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Layout
//===----------------------------------------------------------------------===//
void Layout::sortSectionOrder(const TargetLDBackend& pBackend,
                              const LinkerConfig& pConfig)
{
  typedef std::pair<LDSection*, unsigned int> SectOrder;
  typedef std::vector<SectOrder > SectListTy;
  SectListTy sect_list;
  // get section order from backend
  for (size_t index = 0; index < m_SectionOrder.size(); ++index) {
    sect_list.push_back(
            std::make_pair(
                    m_SectionOrder[index],
                    pBackend.getSectionOrder(*m_SectionOrder[index])
            ));
  }

  // simple insertion sort should be fine for general cases such as so and exec
  for (unsigned int i = 1; i < sect_list.size(); ++i) {
    SectOrder order = sect_list[i];
    int j = i - 1;
    while (j >= 0 && sect_list[j].second > order.second) {
      sect_list[j + 1] = sect_list[j];
      --j;
    }
    sect_list[j + 1] = order;
  }

  // update the sorted ordering to m_SectionOrder
  m_SectionOrder.clear();
  for (size_t index = 0; index < sect_list.size(); ++index) {
    m_SectionOrder.push_back(sect_list[index].first);
  }
}

/// layout - layout the sections
///   1. finalize fragment offset
///   2. compute section order
///   3. finalize section offset
bool Layout::layout(Module& pModule,
                    const TargetLDBackend& pBackend,
                    const LinkerConfig& pConfig)
{
  // determine what sections in output context will go into final output, and
  // push the needed sections into m_SectionOrder for later processing
  Module::iterator it, itEnd = pModule.end();
  for (it = pModule.begin(); it != itEnd; ++it) {
    // calculate 1. all fragment offset, and 2. the section order
    LDSection* sect = *it;

    switch (sect->kind()) {
      // take NULL and StackNote directly
      case LDFileFormat::Null:
      case LDFileFormat::StackNote:
        m_SectionOrder.push_back(sect);
        break;
      // ignore if section size is 0
      case LDFileFormat::Regular:
      case LDFileFormat::Target:
      case LDFileFormat::MetaData:
      case LDFileFormat::BSS:
      case LDFileFormat::Debug:
      case LDFileFormat::EhFrame:
      case LDFileFormat::GCCExceptTable:
      case LDFileFormat::NamePool:
      case LDFileFormat::Relocation:
      case LDFileFormat::Note:
      case LDFileFormat::EhFrameHdr:
        if (0 != sect->size()) {
          m_SectionOrder.push_back(sect);
        }
        break;
      case LDFileFormat::Group:
        if (LinkerConfig::Object == pConfig.codeGenType()) {
          //TODO: support incremental linking
          ;
        }
        break;
      case LDFileFormat::Version:
        if (0 != sect->size()) {
          m_SectionOrder.push_back(sect);
          warning(diag::warn_unsupported_symbolic_versioning) << sect->name();
        }
        break;
      default:
        if (0 != sect->size()) {
          error(diag::err_unsupported_section) << sect->name() << sect->kind();
        }
        break;
    }
  } // end of for

  // perform sorting on m_SectionOrder to get a ordering for final layout
  sortSectionOrder(pBackend, pConfig);

  // Backend defines the section start offset for section 1.
  uint64_t offset = pBackend.sectionStartOffset();

  // compute the section offset and handle alignment also. And ignore section 0
  // (NULL in ELF/COFF), and MachO starts from section 1.
  // always set NULL section's offset to 0
  m_SectionOrder[0]->setOffset(0);
  for (size_t index = 1; index < m_SectionOrder.size(); ++index) {

    if (LDFileFormat::BSS != m_SectionOrder[index - 1]->kind()) {
      // we should not preserve file space for the BSS section.
      offset += m_SectionOrder[index - 1]->size();
    }

    alignAddress(offset, m_SectionOrder[index]->align());
    m_SectionOrder[index]->setOffset(offset);
  }

  // FIXME: Currently Writer bases on the section table in output context to
  // write out sections, so we have to update its content..
  pModule.getSectionTable().clear();
  for (size_t index = 0; index < m_SectionOrder.size(); ++index) {
    pModule.getSectionTable().push_back(m_SectionOrder[index]);
    // after sorting, update the correct output section indices
    m_SectionOrder[index]->setIndex(index);
  }
  return true;
}

