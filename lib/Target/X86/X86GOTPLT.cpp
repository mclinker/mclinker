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

#include <llvm/Support/Casting.h>

#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDFileFormat.h>
#include <mcld/Support/MsgHandling.h>

namespace mcld {

//===----------------------------------------------------------------------===//
// X86GOTPLT
//===----------------------------------------------------------------------===//
X86GOTPLT::X86GOTPLT(LDSection& pSection)
  : GOT(pSection), m_pLast(NULL)
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

void X86GOTPLT::reserve(size_t pNum)
{
  for (size_t i = 0; i < pNum; i++) {
    new X86GOTPLTEntry(0, m_SectionData);
  }
}

X86GOTPLTEntry* X86GOTPLT::consume()
{
  if (NULL == m_pLast) {
    assert(!empty() && "Consume empty GOT entry!");
    m_pLast = llvm::cast<X86GOTPLTEntry>(&m_SectionData->front());
    return m_pLast;
  }

  m_pLast = llvm::cast<X86GOTPLTEntry>(m_pLast->getNextNode());
  return m_pLast;
}

bool X86GOTPLT::hasGOT1() const
{
  return (m_SectionData->size() > X86GOTPLT0Num);
}

void X86GOTPLT::applyGOT0(uint64_t pAddress)
{
  llvm::cast<X86GOTPLTEntry>
    (*(m_SectionData->getFragmentList().begin())).setValue(pAddress);
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
    llvm::cast<X86GOTPLTEntry>(*it).setValue(plt_addr + 6);
    plt_addr += pPLT.getPLT1Size();
  }
}

} //end mcld namespace
