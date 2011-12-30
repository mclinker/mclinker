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

Layout::Layout()
 : m_SectionOrder(), m_InputRangeList(), m_LastValidFragment()
{
}

Layout::~Layout()
{
}

bool Layout::isFragmentUpToDate(const llvm::MCFragment& pFrag) const
{
  const llvm::MCSectionData& sect_data = *pFrag.getParent();
  const llvm::MCFragment* last_valid = m_LastValidFragment.lookup(&sect_data);
  if (!last_valid)
    return false;
  assert(last_valid->getParent() == pFrag.getParent());
  return pFrag.getLayoutOrder() <= last_valid->getLayoutOrder();
}

void Layout::layoutFragment(llvm::MCFragment& pFrag)
{
  llvm::MCFragment* prev = pFrag.getPrevNode();

  // We should never try to recompute something which is up-to-date.
  assert(!isFragmentUpToDate(pFrag) &&
         "Attempt to recompute up-to-date fragment!");
  // We should never try to compute the fragment layout if it's predecessor
  // isn't up-to-date.
  assert((!prev || isFragmentUpToDate(*prev)) &&
         "Attempt to compute fragment before it's predecessor!");

  // Compute fragment offset and size.
  uint64_t offset = 0;
  if (prev)
    offset += prev->Offset + computeFragmentSize(*this, *prev);

  pFrag.Offset = offset;
  m_LastValidFragment[pFrag.getParent()] = &pFrag;
}

void Layout::ensureValid(const llvm::MCFragment& pFrag) const
{
  llvm::MCSectionData& sect_data = *pFrag.getParent();

  llvm::MCFragment* cur = m_LastValidFragment[&sect_data];
  if (!cur)
    cur = &*sect_data.begin();
  else
    cur = cur->getNextNode();

  // Advance the layout position until the fragment is up-to-date.
  while (!isFragmentUpToDate(pFrag)) {
    const_cast<Layout*>(this)->layoutFragment(*cur);
    cur = cur->getNextNode();
  }
}

void Layout::orderRange(llvm::MCFragment* pFront, llvm::MCFragment* pRear)
{
  assert(NULL != pFront && NULL != pRear);

  unsigned int frag_index = 0;
  if (NULL != pFront->getPrevNode())
    frag_index = pFront->getPrevNode()->getLayoutOrder() + 1;

  while (pFront != pRear) {
    pFront->setLayoutOrder(frag_index++);
    pFront = pFront->getNextNode();
  }
}

void Layout::ensureFragmentOrdered(const llvm::MCFragment& pFrag) const
{
  llvm::MCSectionData& sect_data = *pFrag.getParent();
  RangeList* sd_range = m_InputRangeList.find(&sect_data)->second;
  RangeList::iterator it;
  // set layout order of a range of fragments until the order of requested
  // fragment is set
  for (it = sd_range->begin();
       (pFrag.getLayoutOrder() == ~(0U)) && (it != sd_range->end());) {
    llvm::MCFragment* front = it->prevRear;
    if (NULL == front)
      front = sect_data.begin();
    else
      front = front->getNextNode();

    // advance the iterator to get rear fragment in the range
    ++it;
    // bypass if the front is ordered since we order a range of fragments once
    if (front->getLayoutOrder() != ~(0U))
       continue;

    llvm::MCFragment* rear;
    if (it != sd_range->end())
      rear = it->prevRear;
    else
      rear = &sect_data.getFragmentList().back();

    const_cast<Layout*>(this)->orderRange(front, rear);
  }
}

uint64_t Layout::getFragmentOffset(const llvm::MCFragment& pFrag) const
{
  ensureValid(pFrag);
  assert(pFrag.Offset != ~UINT64_C(0) && "Address not set!");
  return pFrag.Offset;
}

/// getFragmentOffset - Get the offset of the given fragment reference inside
/// its containing section.
uint64_t Layout::getFragmentRefOffset(const MCFragmentRef& pFragRef) const
{
  const llvm::MCFragment* frag = pFragRef.frag();
  assert(NULL != frag);

  ensureFragmentOrdered(*frag);
  ensureValid(*frag);

  return frag->Offset + pFragRef.offset();
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

  // simple insertion sort should be fine for general cases
  for (unsigned int i = 1; i < sect_list.size(); ++i) {
    SectOrder order = sect_list[i];
    int j = i - 1;
    while (j >= 0 && sect_list[j].second > order.second) {
      sect_list[j + 1] = sect_list[j];
      --j;
    }
    sect_list[j + 1] = order;
  }

  // update the sorting order to m_SectionOrder
  m_SectionOrder.clear();
  SectListTy::iterator iter;
  for (iter = sect_list.begin(); iter != sect_list.end(); ++iter)
    m_SectionOrder.push_back(iter->first);

}

bool Layout::layout(LDContext& pOutput, const TargetLDBackend& pBackend)
{
  // determine what sections in output context will go into final output, and
  // push the needed sections into m_SectionOrder for later processing
  unsigned int index = 0;
  for (LDContext::sect_iterator it = pOutput.sectBegin();
       it != pOutput.sectEnd(); ++it, ++index) {
    LDSection* sect = pOutput.getSection(index);
    assert(NULL != sect);

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
          ensureFragmentOrdered(frag);
          ensureValid(frag);
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

  // perform sorting on m_SectionOrder to get a final ordering
  sortSectionOrder(pBackend);

  // compute the section offset and addr, and handle alignment also.
  // ignore the section 0 (NULL in ELF/COFF), and MachO starts from section 1
  unsigned int isBSSLaidOut = 0;
  for (index = 1; index < m_SectionOrder.size(); ++index) {
    uint64_t offset;
    LDSection* cur = m_SectionOrder[index];
    LDSection* prev = m_SectionOrder[index - 1];

    // Backend defines the section start offset for section 1.
    if (LDFileFormat::Null != prev->kind())
      offset = prev->offset() + prev->size();
    else
      offset = pBackend.sectionStartOffset();

   alignAddress(offset, pBackend.bitclass());

    // FIXME: if .bss is laid out, set its addr to 0
    if (0 == isBSSLaidOut)
      cur->setAddr(offset);
    else
      cur->setAddr(0);
    cur->setOffset(offset);

    if (LDFileFormat::BSS == cur->kind())
      isBSSLaidOut = 1;
  }

  // FIXME: Currently writer writes sections based on the section table in
  // output context, we have to update its content..
  pOutput.getSectionTable().clear();
  for (index = 0; index < m_SectionOrder.size(); ++index)
    pOutput.getSectionTable().push_back(m_SectionOrder[index]);

  return true;
}

/// getFragmentRef - give a LDSection in input file and an offset, return
/// the fragment reference.
MCFragmentRef Layout::getFragmentRef(const LDSection& pInputSection,
                                     uint64_t pOffset) const
{
  // find out which SectionData covers the range of input section header
  const llvm::MCSectionData* sect_data = pInputSection.getSectionData();
  assert(NULL != sect_data);

  InputRangeList::const_iterator sd_range_iter =
    m_InputRangeList.find(sect_data);
  if (sd_range_iter == m_InputRangeList.end())
    llvm::report_fatal_error(
      llvm::Twine("Attempt to get FragmentRef in section '") +
      pInputSection.name() +
      llvm::Twine("' but without SectionData\n"));

  RangeList* sd_range = sd_range_iter->second;

  // find out the specific part in SectionData range
  RangeList::iterator it, range_end = sd_range->end();
  for (it = sd_range->begin(); it != range_end; ++it) {
    if (&pInputSection == it->header)
      break;
  }
  if (it == range_end)
    llvm::report_fatal_error(
      llvm::Twine("Attempt to find a non-exsiting range from section '") +
      pInputSection.name());

  // get the begin fragment in the range of input section
  const llvm::MCFragment* frag = it->prevRear;
  if (NULL == frag)
    frag = sect_data->begin();
  else
    frag = frag->getNextNode();

  // advance the iterator to get the rear fragment in the interested range
  const llvm::MCFragment* rear = (++it != range_end) ? it->prevRear :
    (&sect_data->getFragmentList().back());
  assert(NULL != frag && NULL != rear);

  // make sure that the order of rear fragment is set
  ensureFragmentOrdered(*rear);

  // make sure that rear fragment and all MCFragments before rear are valid
  ensureValid(*rear);

  uint64_t target_offset = frag->Offset + pOffset;
  // The given offset exceeds the actual range of Section
  if ((rear->Offset + computeFragmentSize(*this, *rear)) < target_offset)
    return MCFragmentRef();

  // find out the target fragment in the range of input section
  while (frag->getNextNode() && (frag->getNextNode()->Offset < target_offset))
    frag = frag->getNextNode();

  return MCFragmentRef(const_cast<llvm::MCFragment&>(*frag),
                       target_offset - frag->Offset);
}

void Layout::addInputRange(const llvm::MCSectionData& pSD,
                           const LDSection& pInputHdr)
{
  // check if mapping the input range to a existing SectionData or a new one
  InputRangeList::iterator sd_range_iter = m_InputRangeList.find(&pSD);
  RangeList* sd_range;
  if (sd_range_iter != m_InputRangeList.end()) {
    sd_range = sd_range_iter->second;
    // check if we already built a mapping for the same input section header
    RangeList::iterator it;
    for (it = sd_range->begin(); it != sd_range->end(); ++it) {
      if (&pInputHdr == it->header)
        llvm::report_fatal_error(llvm::Twine("Trying to map 2nd range of ") +
                                 pInputHdr.name() +
                                 llvm::Twine(" into the same SectionData!\n"));
    }
  } else
    sd_range = m_InputRangeList[&pSD] = new RangeList();

  Range range = {
    &pInputHdr,
    NULL,
  };
  if (!pSD.getFragmentList().empty())
    range.prevRear =
      const_cast<llvm::MCFragment*>(&pSD.getFragmentList().back());
  sd_range->push_back(range);
}

size_t Layout::numOfSections() const
{
  // TODO
  return 0;
}

size_t Layout::numOfSegments() const
{
  // TODO
  return 0;
}

/// getInputLDSection - give a MCFragment, return the corresponding input
/// LDSection*
const LDSection* Layout::getInputLDSection(const llvm::MCFragment& pFrag) const
{
  const llvm::MCSectionData* sect_data = pFrag.getParent();
  if (NULL == sect_data)
    return NULL;

  InputRangeList::const_iterator sd_range_iter =
    m_InputRangeList.find(sect_data);
  if (sd_range_iter == m_InputRangeList.end())
    return NULL;

  ensureFragmentOrdered(sect_data->getFragmentList().back());
  // The LayoutOrder of fragment should be initialized.
  if (~(0U) == pFrag.getLayoutOrder())
    return NULL;

  // Now we should be able to find out the input section that covers the frag
  unsigned int i;
  RangeList& sd_range = *(*sd_range_iter).second;
  for (i = 1; i < sd_range.size(); ++i) {
    const llvm::MCFragment* frag = sd_range[i].prevRear->getNextNode();
    assert(NULL != frag);

    if (frag->getLayoutOrder() == pFrag.getLayoutOrder())
      return sd_range[i].header;
    if (frag->getLayoutOrder() > pFrag.getLayoutOrder())
      return sd_range[i - 1].header;
  }

  // The fragment is in the last input range (section)
  return sd_range[i - 1].header;
}

const LDSection* Layout::getOutputLDSection(const llvm::MCFragment& pFrag) const
{
  llvm::MCSectionData* sect_data = pFrag.getParent();
  if (NULL == sect_data)
    return NULL;

  return static_cast<const LDSection*>(&sect_data->getSection());
}

/// getFragmentRef - give a fragment and a big offset, return the fragment
/// reference in the section data.
///
/// @param pFrag
/// @parem pBigOffset
MCFragmentRef
Layout::getFragmentRef(const llvm::MCFragment& pFrag, uint64_t pBigOffset)
{
  const LDSection* sect = getInputLDSection(pFrag);
  assert(NULL != sect);

  return getFragmentRef(*sect, pBigOffset);
}

/// getFragmentRef - give a fragment and a big offset, return the fragment
/// reference in the section data.
///
/// @param pFrag
/// @parem pBigOffset
const MCFragmentRef
Layout::getFragmentRef(const llvm::MCFragment& pFrag, uint64_t pBigOffset) const
{
  const LDSection* sect = getInputLDSection(pFrag);
  assert(NULL != sect);

  return getFragmentRef(*sect, pBigOffset);
}

