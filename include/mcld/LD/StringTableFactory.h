/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLEFACTORY_H
#define LDSTRINGTABLEFACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{
  class StringTableIF;

/** \class StringTableFactory
 *  \brief StringTableFactory constructs StringTables.
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
template<bool global>
class StringTableFactory
{
public:
  StringTableFactory();
  StringTableIF* create();
};

template
class SymbolTableFactory<true>
{
public:
  StringTableFactory()
  {}

  StringTableIF* createDyn()
  {
    return new DynStrTable(Instance());
  }

  StringTableIF* create()
  {
    return new StrTable(Instance());
  }

private:
  StringTableStorage* Instance()
  {
    static StringTableStorage* instance = new StringTableStorage();
    return instance;
  }
};

} // namespace of mcld

#endif
