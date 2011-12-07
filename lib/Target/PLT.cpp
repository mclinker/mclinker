/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Target/PLT.h>
#include <mcld/Target/GOT.h>

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

