//===- X86GOTPLT.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86GOTPLT.h"
#include "X86PLT.h"

#include <new>

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/Support/MsgHandling.h>

namespace {
  const uint64_t X86GOTPLTEntrySize = 4;
}

namespace mcld {

//===----------------------------------------------------------------------===//
// X86GOTPLT
//===----------------------------------------------------------------------===//
X86GOTPLT::X86GOTPLT(LDSection& pSection)
  : GOT(pSection, X86GOTPLTEntrySize)
{
  // Create GOT0 entries
  reserve(X86GOTPLT0Num);

  // Skip GOT0 entries
  for (size_t i = 0; i < X86GOTPLT0Num; ++i) {
    consume();
  }
}

X86GOTPLT::~X86GOTPLT()
{
}

bool X86GOTPLT::hasGOT1() const
{
  return (m_SectionData->size() > X86GOTPLT0Num);
}

void X86GOTPLT::applyGOT0(uint64_t pAddress)
{
  llvm::cast<Entry>
    (*(m_SectionData->getFragmentList().begin())).setContent(pAddress);
}

void X86GOTPLT::applyAllGOTPLT(const X86PLT& pPLT)
{
  iterator it = begin();
  // skip GOT0
  for (size_t i = 0; i < X86GOTPLT0Num; ++i)
    ++it;
  // address of corresponding plt entry
  uint64_t plt_addr = pPLT.addr() + pPLT.getPLT0Size();
  for (; it != end() ; ++it) {
    llvm::cast<Entry>(*it).setContent(plt_addr + 6);
    plt_addr += pPLT.getPLT1Size();
  }
}

} //end mcld namespace
