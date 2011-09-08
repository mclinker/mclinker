/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef STRINGTABLE_TEST_H
#define STRINGTABLE_TEST_H

#include <gtest.h>

namespace mcld
{
  template <bool Global>
  class StringTableFactory;
  class StringTable;
} // namespace for mcld

namespace mcldtest
{

/** \class StringTableTest
 *  \brief
 *
 *  \see StringTable
 */
class StringTableTest : public ::testing::Test
{
public:
  // Constructor can do set-up work for all test here.
  StringTableTest();

  // Destructor can do clean-up work that doesn't throw exceptions here.
  virtual ~StringTableTest();

  // SetUp() will be called immediately before each test.
  virtual void SetUp();

  // TearDown() will be called immediately after each test.
  virtual void TearDown();

protected:
  mcld::StringTableFactory<true>* m_pFactory;
  mcld::StringTableIF* m_pTestee;
};

} // namespace of mcldtest

#endif
