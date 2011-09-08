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
  class LDStringTableIF;

/** \class LDStringTableFactory
 *  \brief LDStringTableFactory constructs StringTables.
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
template<bool global>
class LDStringTableFactory
{
public:
  LDStringTableFactory();
  LDStringTableIF* create();
};

template
class LDSymbolTableFactory<true>
{
public:
  LDStringTableFactory()
  {}

  LDStringTableIF* createDyn()
  {
    return new LDDynStrTable(Instance());
  }

  LDStringTableIF* create()
  {
    return new LDStrTable(Instance());
  }

private:
  LDStringTableStorage* Instance()
  {
    static LDStringTableStorage* instance = new LDStringTableStorage();
    return instance;
  }
};

} // namespace of mcld

#endif
