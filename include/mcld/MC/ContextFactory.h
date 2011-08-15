/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                          *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_CONTEXT_FACTORY_H
#define MCLD_CONTEXT_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCLDContext.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/Support/RealPath.h>
#include <map>

namespace mcld
{

/** \class ContextFactory
 *  \brief ContextFactory constructs and destructs MCLDContexts. It also
 *  garantee to destruct all its own MCLDContext.
 */
class ContextFactory : public GCFactory<MCLDContext, 0>
{
private:
  typedef GCFactory<MCLDContext, 0> Alloc;
  typedef std::map<sys::fs::RealPath, MCLDContext*> CntxtMap;

public:
  explicit ContextFactory(size_t pNum);
  ~ContextFactory();

  // ----- production  ----- //
  MCLDContext* produce(const sys::fs::Path& pPath);

private:
  CntxtMap m_CntxtMap;
};

} // namespace of mcld

#endif

