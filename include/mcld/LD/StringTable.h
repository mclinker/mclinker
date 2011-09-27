/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLE_H
#define LDSTRINGTABLE_H
#include <mcld/LD/StringTableIF.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class StringTable
 *  \brief String table
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
class StringTable : public StringTableIF
{
public:
  StringTable(StringStorage* pImpl)
  : StringTableIF(pImpl) {}
};

} // namespace of mcld

#endif
