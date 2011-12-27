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

uint64_t Layout::getFragmentOffset(const llvm::MCFragment& pFrag) const
{
  ensureValid(pFrag);
  assert(pFrag.Offset != ~UINT64_C(0) && "Address not set!");
  return pFrag.Offset;
}

/// getFragmentOffset - Get the offset of the given fragment inside its
uint64_t Layout::getFragmentRefOffset(const MCFragmentRef& F) const
{
  return 0; // TODO
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
  // TODO
  return MCFragmentRef();
}

void Layout::addInputRange(const llvm::MCSectionData& pSD, const LDSection& pInputHdr)
{
  // check if mapping the input range to a existing SectionData or a new one
  InputRangeList::iterator sect_data_iter = m_InputRangeList.find(&pSD);
  RangeList* sect_data;
  if (sect_data_iter != m_InputRangeList.end()) {
    sect_data = sect_data_iter->second;
    assert(NULL != sect_data);
    // check if we already built a mapping for the same input section header
    RangeList::iterator it;
    for (it = sect_data->begin(); it != sect_data->end(); ++it) {
      if (&pInputHdr == &(*(it->header)))
        llvm::report_fatal_error(llvm::Twine("Trying to map second range of ") +
                                 pInputHdr.name() +
                                 llvm::Twine(" into the same SectionData!\n"));
    }
  } else {
    sect_data = m_InputRangeList[&pSD] = new RangeList();
    assert(NULL != sect_data);
  }

  Range range = {
    &pInputHdr,
    NULL,
  };
  if (!pSD.getFragmentList().empty())
    range.prevRear =
      const_cast<llvm::MCFragment*>(&pSD.getFragmentList().back());
  sect_data->push_back(range);
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

