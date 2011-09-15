/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#include <iostream>
#include <mcld/LD/StringTableFactory.h>
#include <mcld/LD/StringTable.h>
#include <StringTableTest.h>

using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
StringTableTest::StringTableTest()
{
  // create testee. modify it if need
  m_pFactory = new StringTableFactory<true>();
  m_pTestee = m_pFactory->create();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
StringTableTest::~StringTableTest()
{
  delete m_pFactory;
}

// SetUp() will be called immediately before each test.
void StringTableTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void StringTableTest::TearDown()
{
}

//==========================================================================//
// Testcases
TEST_F(StringTableTest, global_table) {
  StringTableIF* p = m_pFactory->create();
  ASSERT_EQ(p, m_pTestee);
  StringTableIF* p2 = m_pFactory->createDyn();
  StringTableIF* p3 = m_pFactory->createDyn();
  ASSERT_EQ(p2, p3);
  ASSERT_NE(p2, m_pTestee);
  ASSERT_NE(p3, m_pTestee);
}
