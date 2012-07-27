//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Target/GOT.h>

#include <cstring>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GOTEntry
//===----------------------------------------------------------------------===//
GOTEntry::GOTEntry(uint64_t pContent, size_t pEntrySize, SectionData* pParent)
  : TargetFragment(Fragment::Target, pParent),
    f_Content(pContent), m_EntrySize(pEntrySize) {
}

GOTEntry::~GOTEntry()
{
}

//===----------------------------------------------------------------------===//
// GOT
//===----------------------------------------------------------------------===//
GOT::GOT(LDSection& pSection,
         SectionData& pSectionData,
         size_t pEntrySize)
  : m_Section(pSection),
    m_SectionData(pSectionData),
    f_EntrySize(pEntrySize) {
}

GOT::~GOT()
{
}

size_t GOT::getEntrySize() const
{
  return f_EntrySize;
}

