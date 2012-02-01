//===- implTest.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/SymbolCategory.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDSymbol.h>
#include <iostream>
#include "SymbolCategoryTest.h"

using namespace std;
using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
SymbolCategoryTest::SymbolCategoryTest()
{
	// create testee. modify it if need
	m_pTestee = new SymbolCategory();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
SymbolCategoryTest::~SymbolCategoryTest()
{
	delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void SymbolCategoryTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void SymbolCategoryTest::TearDown()
{
}

//==========================================================================//
// Testcases
//

TEST_F(SymbolCategoryTest, upward_test) {
  ResolveInfo* a = m_InfoFactory.produce("a");
  ResolveInfo* b = m_InfoFactory.produce("b");
  ResolveInfo* c = m_InfoFactory.produce("c");
  ResolveInfo* d = m_InfoFactory.produce("d");
  ResolveInfo* e = m_InfoFactory.produce("e");
  e->setBinding(ResolveInfo::Global);
  d->setBinding(ResolveInfo::Weak);
  c->setDesc(ResolveInfo::Common);
  c->setBinding(ResolveInfo::Global);
  b->setBinding(ResolveInfo::Local);
  a->setType(ResolveInfo::File);
  
  LDSymbol aa;
  LDSymbol bb;
  LDSymbol cc;
  LDSymbol dd;
  LDSymbol ee;

  aa.setResolveInfo(*a);
  bb.setResolveInfo(*b);
  cc.setResolveInfo(*c);
  dd.setResolveInfo(*d);
  ee.setResolveInfo(*e);

  m_pTestee->add(ee);
  m_pTestee->add(dd);
  m_pTestee->add(cc);
  m_pTestee->add(bb);
  m_pTestee->add(aa);

  SymbolCategory::iterator sym = m_pTestee->begin();
  ASSERT_STREQ("a", (*sym)->name());
  ++sym;
  ASSERT_STREQ("b", (*sym)->name());
  ++sym;
  ASSERT_STREQ("c", (*sym)->name());
  ++sym;
  ASSERT_STREQ("d", (*sym)->name());
  ++sym;
  ASSERT_STREQ("e", (*sym)->name());

  ASSERT_EQ(2, m_pTestee->numOfLocals());
  ASSERT_EQ(1, m_pTestee->numOfCommons());
  ASSERT_EQ(2, m_pTestee->numOfRegulars());
  ASSERT_EQ(5, m_pTestee->numOfSymbols());
}

