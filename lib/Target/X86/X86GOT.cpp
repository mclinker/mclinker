//===- impl.cpp -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86GOT.h"

#include <mcld/LD/LDFileFormat.h>
#include <mcld/LD/SectionData.h>

#include <llvm/Support/Casting.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// X86GOT
//===----------------------------------------------------------------------===//
X86GOT::X86GOT(LDSection& pSection)
  : GOT(pSection), m_pLast(NULL)
{
}

X86GOT::~X86GOT()
{
}

void X86GOT::reserve(size_t pNum)
{
  for (size_t i = 0; i < pNum; i++) {
    new X86GOTEntry(0, m_SectionData);
  }
}

X86GOTEntry* X86GOT::consume()
{
  if (NULL == m_pLast) {
    assert(!empty() && "Consume empty GOT entry!");
    m_pLast = llvm::cast<X86GOTEntry>(&m_SectionData->front());
    return m_pLast;
  }

  m_pLast = llvm::cast<X86GOTEntry>(m_pLast->getNextNode());
  return m_pLast;
}

