/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 ****************************************************************************/

#ifndef MCLD_PATHCACHE_H
#define MCLD_PATHCACHE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/ADT/HashEntry.h>
#include <mcld/ADT/HashTable.h>
#include <mcld/ADT/StringHash.h>

namespace mcld {
namespace sys  {
namespace fs   {

namespace {
  typedef HashEntry<llvm::StringRef, mcld::sys::fs::Path*, StringCompare<const llvm::StringRef&> > HashEntryType;
} // anonymous namespace

typedef HashTable<HashEntryType, StringHash<BKDR>, EntryFactory<HashEntryType> > PathCache;

} // namespace of fs
} // namespace of sys
} // namespace of mcld

#endif
