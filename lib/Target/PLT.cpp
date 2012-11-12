//===- PLT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Target/PLT.h>
#include <mcld/IRBuilder.h>

using namespace mcld;

class GOT;

//===----------------------------------------------------------------------===//
// PLT::Entry
//===----------------------------------------------------------------------===//
PLT::Entry::Entry(size_t pSize, SectionData& pParent)
  : TargetFragment(Fragment::Target, &pParent),
    m_EntrySize(pSize), m_pContent(NULL)
{
}

PLT::Entry::~Entry()
{
  if (m_pContent) {
    free(m_pContent);
    m_pContent = NULL;
  }
}

//===----------------------------------------------------------------------===//
// PLT
//===----------------------------------------------------------------------===//
PLT::PLT(LDSection& pSection)
  :m_Section(pSection)
{
  m_SectionData = IRBuilder::CreateSectionData(pSection);
}

PLT::~PLT()
{
}

