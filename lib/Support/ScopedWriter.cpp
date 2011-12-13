/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com>                                          *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#include <mcld/Support/ScopedWriter.h>

using namespace mcld;

//==========================
// ScopedRegion
ScopedWriter::ScopedWriter(MemoryRegion *pRegion, bool pSameEndian)
  : m_Region(pRegion),
    m_Buffer(m_Region->getBuffer()),
    m_SameEndian(pSameEndian),
    m_Cursor(0) {
}

