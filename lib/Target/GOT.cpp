/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Target/GOT.h>
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
