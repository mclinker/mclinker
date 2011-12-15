//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/GOT.h"
#include <cstdlib>

using namespace mcld;

//==========================
// GOT
GOTEntry::GOTEntry(unsigned int size, unsigned char* content)
  : llvm::MCFragment(llvm::MCFragment::FT_GOT),
    m_EntrySize(size), m_pContent(content) {
}

GOTEntry::~GOTEntry() {
}
