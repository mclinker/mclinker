//===- Layout.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

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
}

