/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/MemoryAreaFactory.h>

using namespace mcld;

//==========================
// MemoryAreaFactory
MemoryAreaFactory::MemoryAreaFactory(size_t pNum)
  : UniqueGCFactoryBase<sys::fs::Path, MemoryArea, 0>(pNum) {
}

MemoryAreaFactory::~MemoryAreaFactory()
{
}

