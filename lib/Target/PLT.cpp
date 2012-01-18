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
PLTEntry::PLTEntry(const unsigned int size, llvm::MCSectionData* pParent)
   : MCTargetFragment(llvm::MCFragment::FT_Target, pParent),
     m_EntrySize(size), m_pContent(0)
{
}

PLTEntry::~PLTEntry()
{
  if (m_pContent) {
    free(m_pContent);
    m_pContent = 0;
  }
}

//===--------------------------------------------------------------------===//
// PLT
PLT::PLT(LDSection& pSection, llvm::MCSectionData& pSectionData)
  :m_Section(pSection),
   m_SectionData(pSectionData)
{
}

PLT::~PLT()
{
}

