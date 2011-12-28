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
#include <mcld/LD/Layout.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/LDSection.h>
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
  assert(!isFragmentUpToDate(pFrag) && "Attempt to recompute up-to-date fragment!");
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

void Layout::ensureRangeOrdered(const Layout::Range& pRange) const
{
  llvm::MCSectionData* sect_data =
    const_cast<llvm::MCSectionData*>(pRange.header->getSectionData());
  assert(NULL != sect_data);
  RangeList* sd_range = m_InputRangeList.find(sect_data)->second;
  RangeList::iterator it;
  for (it = sd_range->begin(); it != sd_range->end();) {
    llvm::MCFragment* front = it->prevRear;
    if (NULL == front)
      front = sect_data->begin();
    else
      front = front->getNextNode();

    // advance the iterator to get rear fragment in the range
    ++it;
    // bypass if the front is ordered because we order a range of fragments once
    if (front->getLayoutOrder() != ~(0U))
       continue;

    llvm::MCFragment* rear;
    if (it != sd_range->end())
      rear = it->prevRear;
    else
      rear = &sect_data->getFragmentList().back();

    const_cast<Layout*>(this)->orderRange(front, rear);

    // break if the requirement is ensured
    if (pRange.header == it->header)
      break;
  }
}

uint64_t Layout::getFragmentOffset(const llvm::MCFragment& pFrag) const
{
  ensureValid(pFrag);
  assert(pFrag.Offset != ~UINT64_C(0) && "Address not set!");
  return pFrag.Offset;
}

/// getFragmentOffset - Get the offset of the given fragment reference inside its
/// containing section.
uint64_t Layout::getFragmentRefOffset(const MCFragmentRef& pFragRef) const
{
  assert(NULL != pFragRef.frag());
  return pFragRef.frag()->Offset + pFragRef.offset();
}

bool Layout::layout(MCLinker& pLinker)
{
 return 0; // TODO
}

/// getFragmentRef - give a LDSection in input file and an offset, return
/// the fragment reference.
MCFragmentRef Layout::getFragmentRef(const LDSection& pInputSection,
                                     uint64_t pOffset) const
{
  // find out which SectionData covers the range of input section header
  InputRangeList::const_iterator sd_range_iter =
    m_InputRangeList.find(pInputSection.getSectionData());
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

  // make sure that the fragment order in the interested range is set
  ensureRangeOrdered(*it);

  // get the begin fragment in the range of input section
  llvm::MCFragment* frag = it->prevRear;
  if (NULL == frag)
    frag =
      const_cast<llvm::MCFragment*>(&*pInputSection.getSectionData()->begin());
  else
    frag = frag->getNextNode();

  // advance the iterator to get the rear fragment in the interested range
  const llvm::MCFragment* rear = (++it != range_end) ? it->prevRear :
    (&pInputSection.getSectionData()->getFragmentList().back());
  assert(NULL != frag && NULL != rear);

  // make sure that rear fragment and all MCFragments before rear are valid
  ensureValid(*rear);

  uint64_t target_offset = frag->Offset + pOffset;
  if ((rear->Offset + computeFragmentSize(*this, *rear)) < target_offset)
    llvm::report_fatal_error(
      llvm::Twine("The given offset exceeds the actual range of Section '") +
      pInputSection.name() +
      llvm::Twine("'\n"));

  // find out the target fragment in the range of input section
  while (frag->getNextNode() && (frag->getNextNode()->Offset < target_offset))
    frag = frag->getNextNode();

  return MCFragmentRef(*frag, target_offset - frag->Offset);
}

void Layout::addInputRange(const llvm::MCSectionData& pSD, const LDSection& pInputHdr)
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
        llvm::report_fatal_error(llvm::Twine("Trying to map second range of ") +
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

