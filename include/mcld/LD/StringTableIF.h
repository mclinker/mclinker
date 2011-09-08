/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLEIF_H
#define LDSTRINGTABLEIF_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class LDStringTableIF
 *  \brief String table interface.
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
class StringTableIF
{
protected:
  StringTableIF(StringTableStorage* pImpl)
  : f_Storage(pImpl) {}

public:
  virtual ~LDStringTableIF() {}
  const char* insert(llvm::StringRef pStr) { return f_storage.add(pStr); }
  size_t size() const { return f_storage; }

protected:
  StringTableStorage* f_Storage;
};

} // namespace of mcld

#endif
