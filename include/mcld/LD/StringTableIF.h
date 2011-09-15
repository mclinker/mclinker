/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLEIF_H
#define LDSTRINGTABLEIF_H
#include <mcld/LD/StringTableStorage.h>
#include <llvm/ADT/StringRef.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class StringTableIF
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
  virtual ~StringTableIF()
  {
    delete f_Storage;
    f_Storage = NULL;
  }

  const char* insert(llvm::StringRef pStr) { return f_Storage->add(pStr); }
  size_t size() const { return f_Storage->size(); }

  StringTableStorage* getStorage() const  { return f_Storage; }  // For unittest

protected:
  StringTableStorage* f_Storage;
};

} // namespace of mcld

#endif
