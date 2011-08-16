/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                          *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/ContextFactory.h>
#include <utility>

using namespace std;
using namespace mcld;
using namespace mcld::sys::fs;

//===---------------------------------------------------------------------===//
// MCLDContextFactory
ContextFactory::ContextFactory(size_t pNum)
  : GCFactory<MCLDContext,0>(pNum) {
}

ContextFactory::~ContextFactory()
{
}

mcld::MCLDContext* mcld::ContextFactory::produce(const Path& pPath)
{
  CntxtMap::iterator ctx = m_CntxtMap.find(RealPath(pPath));
  if (ctx != m_CntxtMap.end()) // found
    return ctx->second;

  // not found
  MCLDContext* result = Alloc::allocate();
  new (result) MCLDContext();
  m_CntxtMap.insert(make_pair(pPath, result));
  return result;
}

