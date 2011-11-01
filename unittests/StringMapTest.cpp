//===- StringMapTest.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "StringMapTest.h"

using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
StringMapTest::StringMapTest()
{
  // create testee. modify it if need
  m_pTestee = new StringMap<int>();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
StringMapTest::~StringMapTest()
{
  delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void StringMapTest::SetUp()
{
  m_pTestee->GetOrCreateValue("a", 1);
  m_pTestee->GetOrCreateValue("b", 2);
  m_pTestee->GetOrCreateValue("c", 5566);
  m_pTestee->GetOrCreateValue("c", 3);
}

// TearDown() will be called immediately after each test.
void StringMapTest::TearDown()
{
  m_pTestee->clear();
}

//==========================================================================//
// Testcases
//
TEST_F( StringMapTest, size ) {
  ASSERT_EQ(3, m_pTestee->size());
  ASSERT_FALSE(m_pTestee->empty());
}

TEST_F( StringMapTest, traverse ) {
  StringMap<int>::iterator num, nEnd = m_pTestee->end();
  size_t count = 0;
  for (num = m_pTestee->begin(); num!=nEnd; ++num) {
    ++count;
  }
  StringMap<int>::iterator it = m_pTestee->begin();
  ASSERT_EQ(1, *it);
  ++it;
  ASSERT_EQ(2, *it);
  ++it;
  ASSERT_EQ(5566, *it);
  ++it;
  ASSERT_TRUE(it == m_pTestee->end());
  ASSERT_EQ(3, count);
}

TEST_F(StringMapTest, clear) {
  m_pTestee->clear();
  ASSERT_TRUE(m_pTestee->empty());
  ASSERT_EQ(0, m_pTestee->size());
  m_pTestee->GetOrCreateValue("a", 123);
  m_pTestee->GetOrCreateValue("a", -5566);
  ASSERT_EQ(1, m_pTestee->size());
  ASSERT_FALSE(m_pTestee->empty());
}
