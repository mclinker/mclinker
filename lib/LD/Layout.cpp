//===- Layout.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/LD/Layout.h>

#include <cassert>

#include <llvm/ADT/Twine.h>

#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/ADT/SizeTraits.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/LDSection.h>
#include <mcld/Fragment/AlignFragment.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Fragment/NullFragment.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/TargetLDBackend.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Range
//===----------------------------------------------------------------------===//
Layout::Range::Range()
  : header(NULL),
    prevRear(NULL) {
}

Layout::Range::Range(const LDSection& pHdr)
  : header(const_cast<LDSection*>(&pHdr)),
    prevRear(NULL) {
}

Layout::Range::~Range()
{
}

//===----------------------------------------------------------------------===//
// Layout
//===----------------------------------------------------------------------===//
Layout::Layout()
{
}

Layout::~Layout()
{
}

/// addInputRange
///   1. add a new range <pInputHdr, previous rear fragment>
///   2. compute the layout order of all previous ranges.
///   2. compute the layout offset of all previous ranges.
void Layout::addInputRange(const SectionData& pSD,
                           const LDSection& pInputHdr)
{
  RangeList* range_list = NULL;

  // get or create the range_list
  if (pSD.getFragmentList().empty() || 0 == m_SDRangeMap.count(&pSD)) {
    range_list = new RangeList();
    m_SDRangeMap[&pSD] = range_list;
  }
  else {
    range_list = m_SDRangeMap[&pSD];
  }

  // make a range and push it into the range list
  Range* range = new Range(pInputHdr);
  range_list->push_back(range);

  // set up previous rear of the range.
  // FIXME: in current design, we can not add a range before finishing adding
  // fragments in the previous range. If the limitation keeps, we can set
  // prevRear to the last fragment in the SectionData simply.
  //
  // if the pSD's fragment list is empty, the range.prevRear keeps NULL.
  if (!pSD.getFragmentList().empty()) {
    range->prevRear =
                  const_cast<Fragment*>(&pSD.getFragmentList().back());
  }
}

/// getFragmentRef - assume the ragne exist, find the fragment reference
FragmentRef* Layout::getFragmentRef(Layout::Range& pRange, uint64_t pOffset)
{
  if (isEmptyRange(pRange))
    return NULL;

  Fragment* front = getFront(pRange);
  if (NULL == front)
    return NULL;

  Fragment* rear = getRear(pRange);
  if (NULL == rear)
    return NULL;

  return getFragmentRef(*front, *rear, pOffset);
}

// @param pFront is the first fragment in the range.
// @param pRear is the last fragment in the range.
// @pOffset is the offset started from pFront.
FragmentRef*
Layout::getFragmentRef(Fragment& pFront, Fragment& pRear, uint64_t pOffset)
{
  Fragment* front = &pFront;
  Fragment* rear  = &pRear;

  // compute the offset from overall start fragment.
  uint64_t target_offset = pFront.getOffset() + pOffset;

  // from front to rear, find the offset which is as large as possible
  // but smaller than the target_offset.
  while (front != rear) {
    if (Fragment::Alignment == front->getKind()) {
      // alignment fragments were not counted in target_offset.
      // Count in the size of alignment fragmen in target_offset here.
      uint64_t align_size = 0x0;
      if (NULL == front->getNextNode()) {
        // If the alignment fragment is the last fragment, increase
        // the target_offset by the alignment fragment's size.
        align_size = front->size();
      }
      else {
        // If the alignment fragment is not the last fragment, the alignment
        // fragment's size is the distance between the two fragment.
        align_size = front->getNextNode()->getOffset() - front->getOffset();
      }
      target_offset += align_size;
      front = front->getNextNode();
      continue;
    }

    if (target_offset >= front->getNextNode()->getOffset()) {
      front = front->getNextNode();
    }
    else {
      // found
      return FragmentRef::Create(*front, target_offset - front->getOffset());
    }
  }

  if (front == rear) {
    if (Fragment::Alignment == front->getKind())
      return NULL;

    if (!isValidOffset(*front, target_offset))
      return NULL;

    return FragmentRef::Create(*front, target_offset - front->getOffset());
  }
  return NULL;
}

/// getFragmentRef - give a LDSection in input file and an offset, return
/// the fragment reference.
FragmentRef*
Layout::getFragmentRef(const LDSection& pInputSection, uint64_t pOffset)
{
  // find out which SectionData covers the range of input section header
  const SectionData* sect_data = pInputSection.getSectionData();

  // check range list
  if (0 == m_SDRangeMap.count(sect_data))
    return NULL;

  if (sect_data->getFragmentList().empty())
    return NULL;

  RangeList* range_list = m_SDRangeMap[sect_data];

  // find out the specific part in SectionData range
  RangeList::iterator range, rangeEnd = range_list->end();
  for (range = range_list->begin(); range != rangeEnd; ++range) {
    // found the range
    if (&pInputSection == range->header) {
      break;
    }
  }

  // range not found
  if (range == rangeEnd) {
    fatal(diag::err_section_not_laid_out) << pInputSection.name();
  }

  return getFragmentRef(*range, pOffset);
}

/// getFragmentRef - give a fragment and a big offset, return the fragment
/// reference in the section data.
///
/// @param pFrag - the given fragment
/// @param pBigOffset - the offset, can be larger than the fragment, but can
///                     not larger than this input section.
/// @return if found, return the fragment. Otherwise, return NULL.
FragmentRef*
Layout::getFragmentRef(const Fragment& pFrag, uint64_t pBigOffset)
{
  // find out which SectionData covers the range of input section header
  const SectionData* sect_data = pFrag.getParent();

  // check range list
  if (0 == m_SDRangeMap.count(sect_data)) {
    llvm::report_fatal_error(llvm::Twine("SectionData has no") +
                             llvm::Twine(" correponding range list.\n"));
  }

  if (sect_data->getFragmentList().empty())
    return NULL;

  RangeList* range_list = m_SDRangeMap[sect_data];

  // find out the specific part in SectionData range
  uint64_t target_offset = pBigOffset + pFrag.getOffset();

  RangeList::iterator range, rangeEnd = range_list->end();
  for (range = range_list->begin(); range != rangeEnd; ++range) {
    if (isEmptyRange(*range))
      continue;
    if (getRear(*range)->getOffset() >= target_offset) {
      break;
    }
  }

  // range not found
  if (range == rangeEnd) {
    llvm::report_fatal_error(llvm::Twine("the offset is too big that") +
                             llvm::Twine(" never be in the range list.\n"));
  }

  return getFragmentRef(*range, pBigOffset);
}

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
  }

  // perform sorting on m_SectionOrder to get a ordering for final layout
  sortSectionOrder(pBackend, pConfig);

  // Backend defines the section start offset for section 1.
  uint64_t offset = pBackend.sectionStartOffset();

  for (size_t index = 1; index < m_SectionOrder.size(); ++index) {
    // compute the section offset and handle alignment also. And ignore section 0
    // (NULL in ELF/COFF), and MachO starts from section 1.

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

bool Layout::isValidOffset(const Fragment& pFrag, uint64_t pTargetOffset) const
{
  uint64_t size = pFrag.size();
  if (0x0 == size)
    return (pTargetOffset == pFrag.getOffset());

  if (NULL != pFrag.getNextNode())
    return (pTargetOffset >= pFrag.getOffset() && pTargetOffset < pFrag.getNextNode()->getOffset());

  return (pTargetOffset >= pFrag.getOffset() && pTargetOffset < (pFrag.getOffset() + size));
}

