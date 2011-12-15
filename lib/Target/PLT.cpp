//===- PLT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/PLT.h"
#include "mcld/Target/GOT.h"

using namespace mcld;

//===--------------------------------------------------------------------===//
// PLTEntry
PLTEntry::PLTEntry(unsigned int size, unsigned char* content)
  : m_EntrySize(size), m_pContent(content),
    llvm::MCFragment(llvm::MCFragment::FT_PLT)
{
}

PLTEntry::~PLTEntry()
{
  delete m_pContent;
  m_pContent = 0;
}

//===--------------------------------------------------------------------===//
// PLT
PLT::PLT(GOT& pGOT)
  : m_GOT(pGOT) {
}

PLT::~PLT()
{
}

