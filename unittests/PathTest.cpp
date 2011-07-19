/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include "PathTest.h"

using namespace mcld;
using namespace mcld::sys::fs;
using namespace mcldtest;


// Constructor can do set-up work for all test here.
PathTest::PathTest()
{
	// create testee. modify it if need
	m_pTestee = new Path();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
PathTest::~PathTest()
{
	delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void PathTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void PathTest::TearDown()
{
}

//==========================================================================//
// Testcases
//

/** 
TEST_F( PathTest, name of  the testcase for Path ) {
	Write you exercise here
}
**/

