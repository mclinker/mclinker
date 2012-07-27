//===- FragmentTest.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "FragmentTest.h"

#include <mcld/LD/Fragment.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/LDSection.h>

using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
FragmentTest::FragmentTest()
{
}

// Destructor can do clean-up work that doesn't throw exceptions here.
FragmentTest::~FragmentTest()
{
}

// SetUp() will be called immediately before each test.
void FragmentTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void FragmentTest::TearDown()
{
}

//===----------------------------------------------------------------------===//
// Testcases
 
TEST_F( FragmentTest, Fragment_constructor ) {
  LDSection test("test", LDFileFormat::Null, 0, 0);
  SectionData* s = new SectionData(test);
  Fragment* f1 = new Fragment(Fragment::Alignment, s);
  EXPECT_TRUE(1 == s->size());
  Fragment* f2 = new Fragment(Fragment::Alignment, s);
  Fragment* f3 = new Fragment(Fragment::Region, s);
  Fragment* f4 = new Fragment(Fragment::Fillment, s);
  Fragment* f5 = new Fragment(Fragment::Relocation, s);
  Fragment* f6 = new Fragment(Fragment::Target, s);
  EXPECT_TRUE(6 == s->size());

  delete s;
}

TEST_F( FragmentTest, Fragment_trivial_function ) {
  LDSection test("test", LDFileFormat::Null, 0, 0);
  SectionData* s = new SectionData(test);
  Fragment* f = new Fragment(Fragment::Alignment, s);
  
  EXPECT_TRUE(Fragment::Alignment == f->getKind());

  EXPECT_TRUE(~uint64_t(0) == f->getOffset());
  EXPECT_TRUE(~(0U) == f->getLayoutOrder());

  f->setOffset(5566);
  EXPECT_TRUE(5566 == f->getOffset());

  f->setLayoutOrder(5566);
  EXPECT_TRUE(5566 == f->getLayoutOrder());
  
  //always return true
  EXPECT_TRUE(f->classof(new Fragment(Fragment::Region, s)) );
  
  delete s;
}


