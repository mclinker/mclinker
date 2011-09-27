/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLEIF_H
#define LDSTRINGTABLEIF_H
#include <mcld/LD/StringStorage.h>
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
  StringTableIF(StringStorage* pImpl)
  : f_Storage(pImpl) {}

public:
  virtual ~StringTableIF()
  {
    delete f_Storage;
    f_Storage = NULL;
  }

  const char* insert(const char* pStr)
  {
    char* p = f_Storage->allocate(strlen(pStr)+1);
    f_Storage->construct(p, pStr);
    return p;
  }

  size_t size() const { return f_Storage->numOfStrings(); }

  StringStorage* getStorage() const  { return f_Storage; }  // For unittest

protected:
  StringStorage* f_Storage;
};

} // namespace of mcld

#endif
