//===- PLT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Target/PLT.h>

using namespace mcld;

class GOT;

//===----------------------------------------------------------------------===//
// PLTEntry
//===----------------------------------------------------------------------===//
PLTEntry::PLTEntry(size_t pSize, SectionData* pParent)
   : TargetFragment(Fragment::Target, pParent),
     m_EntrySize(pSize), m_pContent(NULL)
{
}

PLTEntry::~PLTEntry()
{
  if (m_pContent) {
    free(m_pContent);
    m_pContent = NULL;
  }
}

//===----------------------------------------------------------------------===//
// PLT
//===----------------------------------------------------------------------===//
PLT::PLT(LDSection& pSection, SectionData& pSectionData)
  :m_Section(pSection),
   m_SectionData(pSectionData)
{
}

PLT::~PLT()
{
}

