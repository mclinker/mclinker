//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDSection.h>
#include <mcld/Target/GOT.h>
#include <cstring>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GOTEntry
GOTEntry::GOTEntry(uint64_t pContent)
  : llvm::MCFragment(llvm::MCFragment::FT_Target, 0),
    f_Content(pContent) {
}

GOTEntry::~GOTEntry()
{
}

//===----------------------------------------------------------------------===//
// GOT
GOT::GOT(LDSection& pSection,
         llvm::MCSectionData& pSectionData,
         const unsigned int pEntryBytes)
  : m_Section(pSection),
    m_SectionData(pSectionData),
    f_EntryBytes(pEntryBytes) {
}

GOT::~GOT()
{
}

unsigned int GOT::entryBytes() const
{
  return f_EntryBytes;
}

