/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLESTORAGE_H
#define LDSTRINGTABLESTORAGE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class LDStringTableStorage : public GCFactory<char, 4*1024>
{
  typedef GCFactory<char, 4*1024> Factory;
public:
  LDStringTableStorage() {}
  ~LDStringTableStorage() {}

  const char* add(llvm::StringRef pSR)
  {
    char* p = Factory.allocate(pSR.size());
    strcpy(p, pSR.data());
    return p;
  }

  size_t size() const { return Factory.size(); }
};

}
