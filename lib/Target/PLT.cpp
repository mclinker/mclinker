//===- PLT.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/Target/PLT.h"

using namespace mcld;

class GOT;

//===--------------------------------------------------------------------===//
// PLTEntry
PLTEntry::PLTEntry(unsigned int size, unsigned char* content)
  : m_EntrySize(size), m_pContent(content),
    llvm::MCFragment(llvm::MCFragment::FT_PLT)
{
}

PLTEntry::~PLTEntry()
{
  if (!m_pContent) {
    delete m_pContent;
    m_pContent = 0;
  }
}

//===--------------------------------------------------------------------===//
// PLT
PLT::PLT(const LDSection& pSection, GOT& pGOTPLT)
  : llvm::MCSectionData(pSection), m_GOTPLT(pGOTPLT)
{
}

PLT::~PLT()
{
}

