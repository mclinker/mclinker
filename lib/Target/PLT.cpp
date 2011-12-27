//===- PLT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/PLT.h"

using namespace mcld;

class GOT;

//===--------------------------------------------------------------------===//
// PLTEntry
PLTEntry::PLTEntry(const unsigned int size)
  : m_EntrySize(size), m_pContent(0),
    llvm::MCFragment(llvm::MCFragment::FT_PLT)
{
}

PLTEntry::~PLTEntry()
{
  if (m_pContent) {
    delete m_pContent;
    m_pContent = 0;
  }
}

//===--------------------------------------------------------------------===//
// PLT
PLT::PLT(llvm::MCSectionData& pSectionData)
  : m_pSectionData(&pSectionData)
{
  assert(m_pSectionData && "m_pSectionData in PLT is NULL!");
}

PLT::~PLT()
{
}

