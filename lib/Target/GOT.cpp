//===- GOT.cpp ------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
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
GOT::GOT(unsigned int pEntrySize)
  : llvm::MCFragment(llvm::MCFragment::FT_GOT),
    m_EntrySize(pEntrySize),
    m_EntryNum(0),
    m_Table(0) {
}

GOT::~GOT()
{
  if (0 != m_Table)
    free(m_Table);
}
