//===- StringUnorderedMapTest.cpp -----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/StringUnorderedMap.h"
#include <StringUnorderedMapTest.h>
#include <string>
#include <cstring>

using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
StringUnorderedMapTest::StringUnorderedMapTest()
{
  m_pTestee = new StringUnorderedMap<int, std::string>();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
StringUnorderedMapTest::~StringUnorderedMapTest()
{
  delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void StringUnorderedMapTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void StringUnorderedMapTest::TearDown()
{
}

//==========================================================================//
// Testcases
//

TEST_F( StringUnorderedMapTest, getOrCreate ) {
  int t;
  t = m_pTestee->getOrCreate("a", 1);
  ASSERT_EQ(1, t);
}

TEST_F( StringUnorderedMapTest, insert_duplicate ) {
  int t;
  t = m_pTestee->getOrCreate("a", 1);
  ASSERT_EQ(1, t);
  t = m_pTestee->getOrCreate("a", 2);
  ASSERT_EQ(1, t);
}

TEST_F( StringUnorderedMapTest, insert_0x00_to_0x7f ) {
  char s[16];
  strcpy(s, "Hello MCLinker.");
  for(int i=0; i<128 ;++i) {
    s[0] = i;
    m_pTestee->getOrCreate(s, i);
  }
  for(int i=127; i>=0 ;--i) {
    s[0] = i;
    int t = m_pTestee->getOrCreate(s);
    ASSERT_EQ(i, t);
  }
  for(int i=0; i<128 ;++i) {
    s[0] = i;
    int t = m_pTestee->getOrCreate(s);
    ASSERT_EQ(i, t);
  }
}
