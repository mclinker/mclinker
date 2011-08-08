/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   MCLDFile <pinronglu@gmail.com>                                          *
 ****************************************************************************/
#ifndef INPUTTREE_TEST_H
#define INPUTTREE_TEST_H

#include <gtest.h>

namespace mcld
{
class InputTree;

} // namespace for mcld

namespace mcldtest
{

/** \class InputTreeTest
 *  \brief 
 *
 *  \see InputTree 
 */
class InputTreeTest : public ::testing::Test
{
public:
	// Constructor can do set-up work for all test here.
	InputTreeTest();

	// Destructor can do clean-up work that doesn't throw exceptions here.
	virtual ~InputTreeTest();

	// SetUp() will be called immediately before each test.
	virtual void SetUp();

	// TearDown() will be called immediately after each test.
	virtual void TearDown();

protected:
	mcld::InputTree* m_pTestee;
};

} // namespace of mcldtest

#endif

