/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/MemoryRegion.h>
#include <iostream>

using namespace std;

using namespace mcld;

//==========================
// MemoryRegion
MemoryRegion::MemoryRegion(MemoryArea::Space *pParentSpace,
                           const MemoryRegion::Address pVMAStart,
                           size_t pSize)
  : m_pParentSpace(pParentSpace), m_VMAStart(pVMAStart), m_Length(pSize) {
}

MemoryRegion::~MemoryRegion()
{
}

void MemoryRegion::sync()
{
  m_pParentSpace->sync();
}

