//===- Layout.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <mcld/ADT/SizeTraits.h>
#include <mcld/LD/Layout.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDContext.h>
#include <mcld/Target/TargetLDBackend.h>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Range
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
Layout::Layout()
  : m_FragRefFactory(32) /** magic number **/ {
}

Layout::~Layout()
{
}

void Layout::setFragmentLayoutOrder(llvm::MCFragment* pFrag)
{
  if (NULL == pFrag)
    return;
  // compute the most-recent fragment whose order was set.
  llvm::MCFragment* first = pFrag;

  while (!hasLayoutOrder(*first)) {
    if (NULL == first->getPrevNode())
      break;
    first = first->getPrevNode();
  }

  // set all layout order
  unsigned int layout_order = 0;
  llvm::MCFragment* frag_not_set = NULL;

  if (NULL == first->getPrevNode()) {
    layout_order = 0;
    frag_not_set = first;
  }
  else {
    layout_order = first->getLayoutOrder();
    frag_not_set = first->getNextNode();
  }

  // set up all layout order
  while(NULL != frag_not_set) {
    frag_not_set->setLayoutOrder(layout_order);
    ++layout_order;
    frag_not_set = frag_not_set->getNextNode();
  }
}

/// setFragmentLayoutOffset - set the fragment's layout offset. This function
/// also set up the layout offsets of all the fragments in the same range.
/// If the offset of the fragment was set before, return immediately.
void Layout::setFragmentLayoutOffset(llvm::MCFragment* pFrag)
{
  if (NULL == pFrag)
    return;
  // compute the most-recent fragment whose offset was set.
  llvm::MCFragment* first = pFrag;

  while (!hasLayoutOffset(*first)) {
    if (NULL == first->getPrevNode())
      break;
    first = first->getPrevNode();
  }

  // set all layout order
  uint64_t offset = 0;
  llvm::MCFragment* frag_not_set = NULL;
  if (NULL == first->getPrevNode()) {
    offset = 0;
    frag_not_set = first;
  }
  else {
    offset = first->Offset;
    offset += computeFragmentSize(*this, *first);
    frag_not_set = first->getNextNode();
  }

  while(NULL != frag_not_set) {
    frag_not_set->Offset = offset;
    offset += computeFragmentSize(*this, *frag_not_set);
    frag_not_set = frag_not_set->getNextNode();
  }
}

/// addInputRange
///   1. add a new range <pInputHdr, previous rear fragment>
///   2. compute the layout order of all previous ranges.
void Layout::addInputRange(const llvm::MCSectionData& pSD,
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
#ifdef MCLD_DEBUG
    RangeList::iterator rangeIter, rangeEnd = range_list->end();
    for (rangeIter = range_list->begin(); rangeIter != rangeEnd; ++rangeIter) {
      if (&pInputHdr == rangeIter->header) {
        llvm::report_fatal_error(llvm::Twine("Trying to map the same LDSection: ") +
                                 pInputHdr.name() +
                                 llvm::Twine(" into the different ranges.\n"));
      }
    }
#endif
  }

  // make a range and push it into the range list
  Range* range = new Range(pInputHdr);
  range_list->push_back(range);

  // set up previous rear of the range.
  // FIXME: in current design, we can not add a range before finishing adding
  // fragments in the previous range. If the limitation keeps, we can set
  // prevRear to the last fragment in the MCSectionData simply.
  //
  // if the pSD's fragment list is empty, the range.prevRear keeps NULL.
  if (!pSD.getFragmentList().empty()) {
    range->prevRear =
                  const_cast<llvm::MCFragment*>(&pSD.getFragmentList().back());
  }
  
  // compute the layout order of the previous range.
  if (!isFirstRange(*range)) {
    setFragmentLayoutOrder(range->prevRear);
    setFragmentLayoutOffset(range->prevRear);
  }
}

/// getInputLDSection - give a MCFragment, return the corresponding input
/// LDSection*
LDSection*
Layout::getInputLDSection(const llvm::MCFragment& pFrag)
{
  const llvm::MCSectionData* sect_data = pFrag.getParent();
  // check the MCSectionData
  if (NULL == sect_data) {
    llvm::report_fatal_error(llvm::Twine("the fragment does not belong to") +
                             llvm::Twine(" any MCSectionData.\n"));
  }

  // check the MCSectionData's range list
  if (0 == m_SDRangeMap.count(sect_data)) {
    llvm::report_fatal_error(llvm::Twine("INTERNAL BACKEND ERROR: ") +
                             llvm::Twine("the input's MCSectionData is not ") +
                             llvm::Twine("registered in the Layout.\nPlease ") +
                             llvm::Twine("use MCLinker::getOrCreateSectData() ") +
                             llvm::Twine("to get input's MCSectionData.\n"));
  }

  RangeList* range_list = m_SDRangeMap[sect_data];
  // the fragment who has the layout order is not in the last range.
  if (hasLayoutOrder(pFrag)) {
    Range range = range_list->back();
    if (isFirstRange(range)) {
      return range.header;
    }
    while(range.prevRear->getLayoutOrder() > pFrag.getLayoutOrder()) {
      if (NULL != range.getPrevNode())
        range = *range.getPrevNode();
      else
        return NULL;
    }
    return range.header;
  }
  // the fragment who has no layout order should be in the last range
  else {
    if (range_list->empty())
      return NULL;
    return range_list->back().header;
  }
}

/// getInputLDSection - give a MCFragment, return the corresponding input
/// LDSection*
const LDSection*
Layout::getInputLDSection(const llvm::MCFragment& pFrag) const
{
  const llvm::MCSectionData* sect_data = pFrag.getParent();
  // check the MCSectionData
  if (NULL == sect_data) {
    llvm::report_fatal_error(llvm::Twine("the fragment does not belong to") +
                             llvm::Twine(" any MCSectionData.\n"));
  }

  // check the MCSectionData's range list
  if (0 == m_SDRangeMap.count(sect_data)) {
    llvm::report_fatal_error(llvm::Twine("INTERNAL BACKEND ERROR: ") +
                             llvm::Twine("the input's MCSectionData is not ") +
                             llvm::Twine("registered in the Layout.\nPlease ") +
                             llvm::Twine("use MCLinker::getOrCreateSectData() ") +
                             llvm::Twine("to get input's MCSectionData.\n"));
  }

  SDRangeMap::const_iterator range_list_iter = m_SDRangeMap.find(sect_data);
  const RangeList* range_list = range_list_iter->second;
  // the fragment who has the layout order is not in the last range.
  if (hasLayoutOrder(pFrag)) {
    Range range = range_list->back();
    if (isFirstRange(range)) {
      return range.header;
    }
    while(range.prevRear->getLayoutOrder() > pFrag.getLayoutOrder()) {
      if (NULL != range.getPrevNode())
        range = *range.getPrevNode();
      else
        return NULL;
    }
    return range.header;
  }
  // the fragment who has no layout order should be in the last range
  else {
    if (range_list->empty())
      return NULL;
    return range_list->back().header;
  }
}

/// getOutputLDSection
LDSection* Layout::getOutputLDSection(const llvm::MCFragment& pFrag)
{
  llvm::MCSectionData* sect_data = pFrag.getParent();
  if (NULL == sect_data)
    return NULL;

  return const_cast<LDSection*>(llvm::cast<LDSection>(&sect_data->getSection()));
}

/// getOutputLDSection
const LDSection* Layout::getOutputLDSection(const llvm::MCFragment& pFrag) const
{
  const llvm::MCSectionData* sect_data = pFrag.getParent();
  if (NULL == sect_data)
    return NULL;

  return llvm::cast<LDSection>(&sect_data->getSection());
}

/// getFragmentRef - assume the ragne exist, find the fragment reference
MCFragmentRef*
Layout::getFragmentRef(Layout::Range& pRange, uint64_t pOffset)
{
  if (isEmptyRange(pRange))
    return NULL;

  llvm::MCFragment* front = getFront(pRange);
  if (NULL == front)
    return NULL;

  llvm::MCFragment* rear = getRear(pRange);
  if (NULL == rear)
    return NULL;

  return getFragmentRef(*front, *rear, pOffset);
}

MCFragmentRef*
Layout::getFragmentRef(llvm::MCFragment& pFront,
                       llvm::MCFragment& pRear,
                       uint64_t pOffset)
{
  uint64_t target_offset = pFront.Offset + pOffset;

  llvm::MCFragment* target_frag = NULL;
  llvm::MCFragment* front = &pFront;
  llvm::MCFragment* rear  = &pRear;

  if (!hasLayoutOffset(*rear)) {
    // compute layout order, offset
    setFragmentLayoutOrder(rear);
    setFragmentLayoutOffset(rear);
  }

  // from front to read, find the offset which is as large as possible
  // but smaller than the target_offset.
  while (front != rear) {
    if (target_offset >= front->getNextNode()->Offset) {
      front = front->getNextNode();
    }
    else {
      // found
      MCFragmentRef* result = m_FragRefFactory.allocate();
      new (result) MCFragmentRef(*front, target_offset - front->Offset);
      return result;
    }
  }

  if (front == rear) {
    if (!isValidOffset(*front, target_offset))
      return NULL;
    MCFragmentRef* result = m_FragRefFactory.allocate();
    new (result) MCFragmentRef(*front, target_offset - front->Offset);
    return result;
  }
  return NULL;
}

/// getFragmentRef - give a LDSection in input file and an offset, return
/// the fragment reference.
MCFragmentRef*
Layout::getFragmentRef(const LDSection& pInputSection, uint64_t pOffset)
{
  // find out which SectionData covers the range of input section header
  const llvm::MCSectionData* sect_data = pInputSection.getSectionData();

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
    llvm::report_fatal_error(llvm::Twine("section ") +
                             pInputSection.name() +
                             llvm::Twine(" never be in the range list.\n"));
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
MCFragmentRef*
Layout::getFragmentRef(const llvm::MCFragment& pFrag, uint64_t pBigOffset)
{
  if (!hasLayoutOffset(pFrag)) {
    // compute layout order, offset
    setFragmentLayoutOrder(const_cast<llvm::MCFragment*>(&pFrag));
    setFragmentLayoutOffset(const_cast<llvm::MCFragment*>(&pFrag));
  }

  // find out which SectionData covers the range of input section header
  const llvm::MCSectionData* sect_data = pFrag.getParent();

  // check range list
  if (0 == m_SDRangeMap.count(sect_data)) {
    llvm::report_fatal_error(llvm::Twine("MCSectionData has no") +
                             llvm::Twine(" correponding range list.\n"));
  }

  if (sect_data->getFragmentList().empty())
    return NULL;

  RangeList* range_list = m_SDRangeMap[sect_data];

  // find out the specific part in SectionData range
  uint64_t target_offset = pBigOffset + pFrag.Offset;

  RangeList::iterator range, rangeEnd = range_list->end();
  for (range = range_list->begin(); range != rangeEnd; ++range) {
    if (isEmptyRange(*range))
      continue;
    if (getRear(*range)->Offset >= target_offset) {
      break;
    }
  }

  // range not found
  if (range == rangeEnd) {
    llvm::report_fatal_error(llvm::Twine("the offset is too big that") +
                             llvm::Twine(" never be in the range list.\n"));
  }

  return getFragmentRef(*range, target_offset);
}

uint64_t Layout::getOutputOffset(const llvm::MCFragment& pFrag)
{
  if (!hasLayoutOffset(pFrag)) {
    // compute layout order, offset
    setFragmentLayoutOrder(const_cast<llvm::MCFragment*>(&pFrag));
    setFragmentLayoutOffset(const_cast<llvm::MCFragment*>(&pFrag));
  }
  return pFrag.Offset;
}

uint64_t Layout::getOutputOffset(const llvm::MCFragment& pFrag) const
{
  if (!hasLayoutOffset(pFrag)) {
    llvm::report_fatal_error(llvm::Twine("INTERNAL BACKEND ERROR: ") +
                             llvm::Twine("the function ") +
                             llvm::Twine(__func__) +
                             llvm::Twine(" can not be used before layout().\n"));
  }
  return pFrag.Offset;
}

uint64_t Layout::getOutputOffset(const MCFragmentRef& pFragRef)
{
  return getOutputOffset(*(pFragRef.frag())) + pFragRef.offset();
}

uint64_t Layout::getOutputOffset(const MCFragmentRef& pFragRef) const
{
  return getOutputOffset(*(pFragRef.frag())) + pFragRef.offset();
}

void Layout::sortSectionOrder(const TargetLDBackend& pBackend)
{
  typedef std::pair<LDSection*, unsigned int> SectOrder;
  typedef std::vector<SectOrder > SectListTy;
  SectListTy sect_list;
  // get section order from backend
  for (sect_iterator it = sect_begin(); it != sect_end(); ++it) {
    sect_list.push_back(std::make_pair(*it, pBackend.getSectionOrder(**it)));
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
  SectListTy::iterator iter;
  for (iter = sect_list.begin(); iter != sect_list.end(); ++iter)
    m_SectionOrder.push_back(iter->first);

}

bool Layout::layout(LDContext& pOutput, const TargetLDBackend& pBackend)
{
  // determine what sections in output context will go into final output, and
  // push the needed sections into m_SectionOrder for later processing
  LDContext::sect_iterator it, itEnd = pOutput.sectEnd();
  for (it = pOutput.sectBegin(); it != itEnd; ++it) {
    LDSection* sect = *it;

    switch (sect->kind()) {
      // ignore if there is no SectionData for certain section kinds
      case LDFileFormat::Regular:
      case LDFileFormat::Note:
      case LDFileFormat::Debug:
      case LDFileFormat::Target:
      case LDFileFormat::MetaData:
        if (NULL != sect->getSectionData()) {
          // make sure that all fragments are valid, and compute section size
          llvm::MCFragment& frag =
            sect->getSectionData()->getFragmentList().back();
          setFragmentLayoutOrder(&frag);
          setFragmentLayoutOffset(&frag);
          sect->setSize(frag.Offset + computeFragmentSize(*this, frag));
          m_SectionOrder.push_back(sect);
        }
        break;
      // take BSS and NULL directly
      case LDFileFormat::BSS:
      case LDFileFormat::Null:
        m_SectionOrder.push_back(sect);
        break;
      // ignore if section size is 0 for NamePool and Relocation
      case LDFileFormat::NamePool:
      case LDFileFormat::Relocation:
        if (0 != sect->size())
          m_SectionOrder.push_back(sect);
        break;
      default:
        assert(0 && "Unknown section kind");
        break;
    }
  }

  // perform sorting on m_SectionOrder to get a ordering for final layout
  sortSectionOrder(pBackend);

  // compute the section offset and handle alignment also. And ignore section 0
  // (NULL in ELF/COFF), and MachO starts from section 1.
  for (size_t index = 1; index < m_SectionOrder.size(); ++index) {
    uint64_t offset;
    LDSection* cur = m_SectionOrder[index];
    LDSection* prev = m_SectionOrder[index - 1];

    // Backend defines the section start offset for section 1.
    if (LDFileFormat::Null != prev->kind())
      offset = prev->offset() + prev->size();
    else
      offset = pBackend.sectionStartOffset();

    // align the offset to target-defined alignment
    alignAddress(offset, pBackend.bitclass());

    cur->setOffset(offset);
  }

  // FIXME: Currently Writer bases on the section table in output context to
  // write out sections, so we have to update its content..
  pOutput.getSectionTable().clear();
  for (size_t index = 0; index < m_SectionOrder.size(); ++index)
    pOutput.getSectionTable().push_back(m_SectionOrder[index]);

  return true;
}

