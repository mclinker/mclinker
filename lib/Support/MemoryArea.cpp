/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>

using namespace mcld;

//===--------------------------------------------------------------------===//
// MemoryArea::Space
MemoryArea::Space::~Space()
{
  // FIXME: switch from ownership.
}


//===--------------------------------------------------------------------===//
// MemoryArea
MemoryArea::MemoryArea()
{
}

MemoryArea::~MemoryArea()
{
}

MemoryRegion* MemoryArea::claim(MemoryArea::Address pStart, size_t pLength)
{
}

void MemoryArea::open(const std::string& pName, int flags, int mode)
{
}

void MemoryArea::close()
{
}

bool MemoryArea::isGood() const
{
}

MemoryArea::Space* MemoryArea::find(MemoryArea::Address pStart, size_t pLength)
{
}


