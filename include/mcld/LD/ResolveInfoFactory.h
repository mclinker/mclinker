//===- ResolveInfoFactory.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_RESOLVE_INFO_FACTORY_H
#define MCLD_RESOLVE_INFO_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/LD/ResolveInfo.h"

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

