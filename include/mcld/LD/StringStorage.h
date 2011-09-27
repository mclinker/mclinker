/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLESTORAGE_H
#define LDSTRINGTABLESTORAGE_H
#include <mcld/Support/GCFactory.h>
#include <llvm/ADT/StringRef.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

class StringStorage : public GCFactory<char, 4*1024>
{
  typedef GCFactory<char, 4*1024> Factory;

public:
  StringStorage()
  : m_Size(0) {}
  ~StringStorage() {}

  char* allocate(size_t pLength)
  {
    char* result = Factory::allocate(pLength+1);
    ++m_Size;
    return result;
  }

  void construct(char*& pAddr, const char* pValue)
  {
    strcpy(pAddr, pValue);
  }

  size_t numOfStrings() const { return m_Size; }

private:
  size_t m_Size;
};

}

#endif
