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
  m_pFactory = new ResolveInfoFactory();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
StaticResolverTest::~StaticResolverTest()
{
  delete m_pResolver;
  delete m_pFactory;
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
  ResolveInfo* old_sym = m_pFactory->produce("abc");
  ResolveInfo* new_sym = m_pFactory->produce("abc");
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

TEST_F( StaticResolverTest, OverrideWeakByGlobal )
{
  ResolveInfo* old_sym = m_pFactory->produce("abc");
  ResolveInfo* new_sym = m_pFactory->produce("abc");
  
  new_sym->setBinding(ResolveInfo::Global);
  old_sym->setBinding(ResolveInfo::Weak);
  new_sym->setSize(0);
  old_sym->setSize(1);
  new_sym->setValue(0);
  old_sym->setValue(1);

  ASSERT_EQ( mcld::ResolveInfo::Global, new_sym->binding());
  ASSERT_EQ( mcld::ResolveInfo::Weak, old_sym->binding());

  ASSERT_TRUE( mcld::ResolveInfo::global_flag == new_sym->info());
  ASSERT_TRUE( mcld::ResolveInfo::weak_flag == old_sym->info());
  bool override = false;
  unsigned int result = m_pResolver->resolve(*old_sym, *new_sym, override);
  ASSERT_EQ( Resolver::Success, result);
  ASSERT_TRUE( override );
  ASSERT_EQ(0, old_sym->size());
  ASSERT_EQ(0, old_sym->value());
}

TEST_F( StaticResolverTest, MarkByBiggerCommon )
{
  ResolveInfo* old_sym = m_pFactory->produce("abc");
  ResolveInfo* new_sym = m_pFactory->produce("abc");
  
  new_sym->setType(ResolveInfo::Common);
  old_sym->setType(ResolveInfo::Common);
  new_sym->setSize(999);
  old_sym->setSize(0);
  new_sym->setValue(999);
  old_sym->setValue(111);

  ASSERT_EQ( mcld::ResolveInfo::Common, new_sym->type());
  ASSERT_EQ( mcld::ResolveInfo::Common, old_sym->type());

  ASSERT_TRUE( mcld::ResolveInfo::common_flag == new_sym->info());
  ASSERT_TRUE( mcld::ResolveInfo::common_flag == old_sym->info());
  bool override = true;
  unsigned int result = m_pResolver->resolve(*old_sym, *new_sym, override);
  ASSERT_EQ( Resolver::Success, result);
  ASSERT_FALSE( override );
  ASSERT_EQ(999, old_sym->size());
  ASSERT_EQ(111, old_sym->value());
}

TEST_F( StaticResolverTest, OverrideByBiggerCommon )
{
  ResolveInfo* old_sym = m_pFactory->produce("abc");
  ResolveInfo* new_sym = m_pFactory->produce("abc");
  
  new_sym->setType(ResolveInfo::Common);
  old_sym->setType(ResolveInfo::Common);
  old_sym->setBinding(ResolveInfo::Weak);
  new_sym->setSize(999);
  old_sym->setSize(0);
  new_sym->setValue(999);
  old_sym->setValue(111);

  ASSERT_EQ( ResolveInfo::Common, new_sym->type());
  ASSERT_EQ( ResolveInfo::Common, old_sym->type());
  ASSERT_EQ( ResolveInfo::Weak, old_sym->binding());

  ASSERT_TRUE( ResolveInfo::common_flag == new_sym->info());
  ASSERT_TRUE( (ResolveInfo::weak_flag | ResolveInfo::common_flag) == old_sym->info());

  bool override = false;
  unsigned int result = m_pResolver->resolve(*old_sym, *new_sym, override);
  ASSERT_EQ( Resolver::Success, result);
  ASSERT_TRUE( override );
  ASSERT_EQ(999, old_sym->size());
  ASSERT_EQ(999, old_sym->value());
}

TEST_F( StaticResolverTest, OverrideCommonByDefine)
{
  ResolveInfo* old_sym = m_pFactory->produce("abc");
  ResolveInfo* new_sym = m_pFactory->produce("abc");
  
  old_sym->setType(ResolveInfo::Common);
  old_sym->setSize(0);
  old_sym->setValue(111);

  new_sym->setType(ResolveInfo::Define);
  new_sym->setSize(999);
  new_sym->setValue(999);

  ASSERT_EQ( ResolveInfo::Define, new_sym->type());
  ASSERT_EQ( ResolveInfo::Common, old_sym->type());

  ASSERT_TRUE( ResolveInfo::define_flag == new_sym->info());
  ASSERT_TRUE( ResolveInfo::common_flag == old_sym->info());

  bool override = false;
  unsigned int result = m_pResolver->resolve(*old_sym, *new_sym, override);
  ASSERT_EQ(Resolver::Warning, result);
  ASSERT_TRUE( override );
  ASSERT_EQ(999, old_sym->size());
  ASSERT_EQ(999, old_sym->value());
  
  ASSERT_STREQ("definition of 'abc' is overriding common.", m_pResolver->mesg().c_str() );

}

