//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/LDSection.h"
#include "mcld/Target/GOT.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// GOT
GOT::GOT(const std::string pSectionName)
  : m_Section(LDFileFormat::GOT, pSectionName)
{
}

GOT::~GOT()
{
}

LDSection* GOT::getSection()
{
  return &m_Section;
}

const LDSection* GOT::getSection() const
{
  return &m_Section;
}

//===----------------------------------------------------------------------===//
// GOTEntry
GOTEntry::GOTEntry(unsigned int size, unsigned char* content)
  : llvm::MCFragment(llvm::MCFragment::FT_GOT),
    m_EntrySize(size), m_pContent(content) {
}

GOTEntry::~GOTEntry() {
}
