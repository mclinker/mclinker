/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#include <mcld/LD/StringTable.h>
#include <StringTableTest.h>

using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
StringTableTest::StringTableTest()
{
  // create testee. modify it if need
  m_pTestee = new StringTable();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
StringTableTest::~StringTableTest()
{
  delete m_pTestee;
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
TEST_F(StringTableTest, add_and_get) {
  m_pTestee->addCString("");
  m_pTestee->addCString("abc");
  m_pTestee->addCString("5566");
  ASSERT_EQ(10, m_pTestee->size());
  ASSERT_STREQ("", m_pTestee->getCString(0));
  ASSERT_STREQ("abc", m_pTestee->getCString(1));
  ASSERT_STREQ("", m_pTestee->getCString(4));
  ASSERT_STREQ("5566", m_pTestee->getCString(5));
}
