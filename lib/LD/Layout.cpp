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

using namespace mcld;

Layout::Layout()
{
}

Layout::~Layout()
{
}

uint64_t Layout::layoutFragment(llvm::MCFragment& pFrag)
{
  return 0; // TODO
}

uint64_t Layout::getFragmentOffset(const llvm::MCFragment& F) const
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

