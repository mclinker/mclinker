//===- UniqueGCFactoryBaseTest.cpp ----------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/ContextFactory.h"
#include "mcld/Support/MemoryAreaFactory.h"
#include "UniqueGCFactoryBaseTest.h"

using namespace mcld;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
UniqueGCFactoryBaseTest::UniqueGCFactoryBaseTest()
{
}

// Destructor can do clean-up work that doesn't throw exceptions here.
UniqueGCFactoryBaseTest::~UniqueGCFactoryBaseTest()
{
}

// SetUp() will be called immediately before each test.
void UniqueGCFactoryBaseTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void UniqueGCFactoryBaseTest::TearDown()
{
}

//==========================================================================//
// Testcases
//
TEST_F( UniqueGCFactoryBaseTest, number_constructor ) {
	ContextFactory *contextFactory = new ContextFactory(10); 
	contextFactory->produce("/");
	contextFactory->produce("ab/c");
	ASSERT_EQ( 2, contextFactory->size());
	delete contextFactory;
}

TEST_F( UniqueGCFactoryBaseTest, unique_produce ) {
	ContextFactory *contextFactory = new ContextFactory(10); 
	LDContext* context1 = contextFactory->produce("/");
	contextFactory->produce("ab/c");
	ASSERT_EQ( 2, contextFactory->size());
	LDContext* context2 = contextFactory->produce("/");
	ASSERT_EQ( context1, context2 );
	delete contextFactory;
}

TEST_F( UniqueGCFactoryBaseTest, unique_produce2 ) {
	ContextFactory *contextFactory = new ContextFactory(10); 
	LDContext* context1 = contextFactory->produce("abc/def");
	contextFactory->produce("ab/c");
	ASSERT_EQ( 2, contextFactory->size());
	LDContext* context2 = contextFactory->produce("ttt/../abc/def");
	ASSERT_EQ( context1, context2 );
	delete contextFactory;
}

TEST_F( UniqueGCFactoryBaseTest, iterator )
{
	MemoryAreaFactory* memFactory = new MemoryAreaFactory(10);
	MemoryArea* area1 = memFactory->produce("/home/luba", O_RDONLY);
	MemoryArea* area2 = memFactory->produce("/home/jush", O_RDONLY);
	ASSERT_NE( area1, area2);
	MemoryArea* area3 = memFactory->produce("/home/jush/../luba", O_RDONLY);
	ASSERT_EQ( area1, area3);
	ASSERT_FALSE( memFactory->empty());
	ASSERT_EQ( 2, memFactory->size());
	MemoryAreaFactory::iterator aIter = memFactory->begin();
	ASSERT_EQ( area1, &(*aIter));
	++aIter;
	ASSERT_EQ( area2, &(*aIter));
	++aIter;
	MemoryAreaFactory::iterator aEnd = memFactory->end();
	ASSERT_TRUE( aEnd == aIter);
	delete memFactory;
}

