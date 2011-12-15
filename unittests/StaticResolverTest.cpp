//===- implTest.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/StaticResolver.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/ResolveInfoFactory.h>
#include "StaticResolverTest.h"

#include <iostream>

using namespace std;
using namespace mcld;
using namespace mcldtest;

// Constructor can do set-up work for all test here.
StaticResolverTest::StaticResolverTest()
{
	// create testee. modify it if need
	m_pResolver = new StaticResolver();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
StaticResolverTest::~StaticResolverTest()
{
	delete m_pResolver;
}

// SetUp() will be called immediately before each test.
void StaticResolverTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void StaticResolverTest::TearDown()
{
}

//==========================================================================//
// Testcases
//
TEST_F( StaticResolverTest, MDEF ) {
  ResolveInfoFactory* factory = new ResolveInfoFactory();
  ResolveInfo* old_sym = factory->produce("abc");
  ResolveInfo* new_sym = factory->produce("abc");
  new_sym->setType(ResolveInfo::Define);
  old_sym->setType(ResolveInfo::Define);
  ASSERT_EQ( mcld::ResolveInfo::Define, new_sym->type());
  ASSERT_EQ( mcld::ResolveInfo::Define, old_sym->type());
  ASSERT_TRUE( mcld::ResolveInfo::define_flag == new_sym->info());
  ASSERT_TRUE( mcld::ResolveInfo::define_flag == old_sym->info());
  bool override = true;
  unsigned int result = m_pResolver->resolve(*old_sym, *new_sym, override);
  ASSERT_EQ( Resolver::Abort, result);
  ASSERT_FALSE( override );
  ASSERT_STREQ( "multiple definitions of `abc'.", m_pResolver->mesg().c_str() );
}

TEST_F( StaticResolverTest, OverrideWeakByStong )
{
}
