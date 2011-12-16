//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Target/GOT.h>
#include <mcld/LD/LDSection.h>
#include <cstdlib>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GOT
GOT::GOT(LDSection* pSection)
  : m_pSection(pSection) {
}

GOT::~GOT()
{
  if (0 != m_pSection)
    delete m_pSection;
}

LDSection& GOT::getSection()
{
  return *m_pSection;
}

const LDSection& GOT::getSection() const
{
  return *m_pSection;
}

//===----------------------------------------------------------------------===//
// GOTEntry
GOTEntry::GOTEntry(unsigned int size, unsigned char* content)
  : llvm::MCFragment(llvm::MCFragment::FT_GOT),
    m_EntrySize(size), m_pContent(content) {
}

GOTEntry::~GOTEntry() {
}
