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
#include <mcld/Support/UniqueGCFactory.h>
#include <mcld/MC/MCLDContext.h>
#include <mcld/Support/Path.h>

namespace mcld
{

/** \class ContextFactory
 *  \brief ContextFactory avoids the duplicated MCLDContext of the same file.
 *
 *  MCLinker is designed for efficient memory usage. Because user can give 
 *  MCLinker the same input file many times on the command line, MCLinker must
 *  avoid opening identical file twice.
 *
 *  ContextFactory is the guard to prevent redundant opening. MCLinker does not
 *  create MCLDContext directly. Instead, it creates MCLDContext by ContextFactory.
 *  ContextFactory returns the identical reference of MCLDContext if it's openend.
 *
 *  @see MCLDContext
 *  @see UniqueGCFactoryBase
 */
class ContextFactory : public UniqueGCFactoryBase<sys::fs::Path, MCLDContext, 0>
{
public:
  explicit ContextFactory(size_t pNum);
  ~ContextFactory();

  MCLDContext* produce(const sys::fs::Path& pPath);
};

} // namespace of mcld

#endif

