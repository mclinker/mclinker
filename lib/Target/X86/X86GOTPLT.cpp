//===- X86GOTPLT.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86GOTPLT.h"

#include <new>

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDFileFormat.h>
#include <mcld/Support/MsgHandling.h>

namespace {
  const uint64_t X86GOTPLTEntrySize = 4;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// X86GOTPLT
//===----------------------------------------------------------------------===//
X86GOTPLT::X86GOTPLT(LDSection& pSection, SectionData& pSectionData)
  : GOT(pSection, pSectionData, X86GOTPLTEntrySize)
{
  // Create GOT0 entries
  reserveEntry(X86GOTPLT0Num);

  // Skip GOT0 entries
  for (size_t i = 0; i < X86GOTPLT0Num; ++i) {
    consumeEntry();
  }
}

X86GOTPLT::~X86GOTPLT()
{
}

void X86GOTPLT::applyGOT0(uint64_t pAddress)
{
  llvm::cast<GOTEntry>
    (*(m_SectionData.getFragmentList().begin())).setContent(pAddress);
}

void X86GOTPLT::applyAllGOTPLT(uint64_t pPLTBase,
                               unsigned int pPLT0Size,
                               unsigned int pPLT1Size)
{
  iterator it = begin();
  // skip GOT0
  for (size_t i = 0; i < X86GOTPLT0Num; ++i)
    ++it;
  // address of corresponding plt entry
  uint64_t plt_addr = pPLTBase + pPLT0Size;
  for (; it != end() ; ++it) {
    llvm::cast<GOTEntry>(*it).setContent(plt_addr + 6);
    plt_addr += pPLT1Size;
  }
}

} //end mcld namespace
