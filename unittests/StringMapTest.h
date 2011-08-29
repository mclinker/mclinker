/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_STRINGMAP_TEST_H
#define MCLD_STRINGMAP_TEST_H

#include <mcld/ADT/StringMap.h>
#include <gtest.h>

namespace mcldtest
{

/** \class StringMapTest
 *  \brief StringMap seems never go to the end.
 *
 *  \see StringMap 
 */
class StringMapTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	StringMapTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~StringMapTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

protected:
	mcld::StringMap<int>* m_pTestee;
};

} // namespace of mcldtest

#endif

