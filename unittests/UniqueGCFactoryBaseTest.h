/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef UNIQUE_GCFACTORYBASE_TEST_H
#define UNIQUE_GCFACTORYBASE_TEST_H

#include <mcld/Support/UniqueGCFactory.h>
#include <gtest.h>

namespace mcldtest
{

/** \class UniqueGCFactoryBaseTest
 *  - check the unique of key.
 *  - make sure the key associates with the same storage of value.
 *  - check if all functions in the GCFactoryBase are available.
 */
class UniqueGCFactoryBaseTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	UniqueGCFactoryBaseTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~UniqueGCFactoryBaseTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();
};

} // namespace of mcldtest

#endif

