/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                          *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/ContextFactory.h>

using namespace mcld;

//===---------------------------------------------------------------------===//
// MCLDContextFactory
ContextFactory::ContextFactory(size_t pNum)
  : UniqueGCFactoryBase<sys::fs::Path, MCLDContext, 0>(pNum) {
}

ContextFactory::~ContextFactory()
{
}

MCLDContext* ContextFactory::produce(const sys::fs::Path& pPath)
{
  MCLDContext* result = find(pPath);
  if (0 == result) {
    result = UniqueGCFactoryBase<sys::fs::Path, MCLDContext, 0>::allocate();
    UniqueGCFactoryBase<sys::fs::Path, MCLDContext, 0>::construct(result);
    f_KeyMap.insert(std::make_pair(pPath, result));
  }
  return result;
}

