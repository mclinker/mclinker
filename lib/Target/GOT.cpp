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
GOTEntry::GOTEntry(uint64_t pContent, uint64_t pEntrySize)
  : MCTargetFragment(llvm::MCFragment::FT_Target, 0),
    f_Content(pContent), m_EntrySize(pEntrySize) {
}

GOTEntry::~GOTEntry()
{
}

//===----------------------------------------------------------------------===//
// GOT
GOT::GOT(LDSection& pSection,
         llvm::MCSectionData& pSectionData,
         const unsigned int pEntrySize)
  : m_Section(pSection),
    m_SectionData(pSectionData),
    f_EntrySize(pEntrySize) {
}

GOT::~GOT()
{
}

uint64_t GOT::getEntrySize() const
{
  return f_EntrySize;
}

