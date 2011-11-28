/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_RESOLVE_INFO_FACTORY_H
#define MCLD_RESOLVE_INFO_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/ResolveInfo.h>

namespace mcld
{

/** \class ResolveInfoFactory
 *  \brief ResolveInfoFactory creates ResolveInfos.
 */
class ResolveInfoFactory
{
public:
  typedef ResolveInfo           entry_type;
  typedef ResolveInfo::key_type key_type;

public:
  entry_type* produce(const key_type& pKey);
  void destroy(entry_type* pEntry);

};

} // namespace of mcld

#endif

