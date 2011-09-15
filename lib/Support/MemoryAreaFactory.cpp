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

MemoryArea* MemoryAreaFactory::produce(const sys::fs::Path& pPath, int pFlags)
{
  MemoryArea* result = find(pPath);
  if (0 == result) {
    result = allocate();
    new (result) MemoryArea(pPath);
    f_KeyMap.insert(std::make_pair(pPath, result));
  }
  result->open(pPath, pFlags);
  return result;
}

MemoryArea* MemoryAreaFactory::produce(const sys::fs::Path& pPath, int pFlags, mode_t pMode)
{
  MemoryArea* result = find(pPath);
  if (0 == result) {
    result = allocate();
    new (result) MemoryArea(pPath);
    f_KeyMap.insert(std::make_pair(pPath, result));
  }
  result->open(pPath, pFlags, pMode);
  return result;
}

