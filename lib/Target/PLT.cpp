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
PLTEntry::PLTEntry()
  : llvm::MCFragment(llvm::MCFragment::FT_PLT)
{
}

PLT::Entry::~PLTEntry()
{
}

//===--------------------------------------------------------------------===//
// PLT
PLT::PLT(GOT& pGOT)
  : m_GOT(pGOT) {
}

PLT::~PLT()
{
}

