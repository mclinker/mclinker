/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDDYNSTRTABLE_H
#define LDDYNSTRTABLE_H
#include <mcld/LD/StringTableIF.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class DynStrTable
 *  \brief Dynamic String table
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
class DynStrTable : public StringTableIF
{
public:
  DynStrTable(StringTableStorage* pImpl)
  : StringTableIF(pImpl) {}
};

} // namespace of mcld

#endif
