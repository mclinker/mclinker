/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCOBJECTREADER_TEST_H
#define MCOBJECTREADER_TEST_H

#include <gtest.h>

namespace mcld
{
class MCObjectReader;

} // namespace for mcld

namespace mcldtest
{

/** \class MCObjectReaderTest
 *  \brief Test MCobjectReader
 *
 *  \see MCObjectReader 
 */
class MCObjectReaderTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	MCObjectReaderTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~MCObjectReaderTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

protected:
	mcld::MCObjectReader* m_pTestee;
};

} // namespace of mcldtest

#endif

