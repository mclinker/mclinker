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

uint64_t Layout::getFragmentOffset(const llvm::MCFragment *F) const
{
  return 0; // TODO
}

bool Layout::layout(MCLinker& pLinker)
{
 return 0; // TODO
}

