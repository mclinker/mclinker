/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLEFACTORY_H
#define LDSTRINGTABLEFACTORY_H
#include <mcld/LD/StringStorage.h>
#include <mcld/LD/StringTable.h>
#include <mcld/LD/DynStrTable.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class StringTableFactory
 *  \brief StringTableFactory constructs StringTables.
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
template <bool global>
class StringTableFactory
{
};

template <>
class StringTableFactory<true> : private Uncopyable
{
public:
  StringTableFactory()
  {}

  StringTableIF* createDyn()
  {
    static DynStrTable Tab(new StringStorage());
    return &Tab;
  }

  StringTableIF* create()
  {
    static StringTable Tab(new StringStorage());
    return &Tab;
  }
};

} // namespace of mcld

#endif
