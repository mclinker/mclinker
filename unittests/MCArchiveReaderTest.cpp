/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCArchiveReader.h>
#include <mcld/Target/TargetArchiveReader.h>
#include <MCArchiveReaderTest.h>

#include <iostream>

using namespace mcld;
using namespace mcldtest;

TargetArchiveReader *TestFunction(void){
	std::cout << " execute test function \n" ;
	return new TargetArchiveReader();
}


// Constructor can do set-up work for all test here.
MCArchiveReaderTest::MCArchiveReaderTest()
{
	// create testee. modify it if need
	m_pTestee = new MCArchiveReader(TestFunction);
}

// Destructor can do clean-up work that doesn't throw exceptions here.
MCArchiveReaderTest::~MCArchiveReaderTest()
{
	delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void MCArchiveReaderTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void MCArchiveReaderTest::TearDown()

{
}

//==========================================================================//
// Testcases
//

TEST_F( MCArchiveReaderTest, test_behavior ) {
	EXPECT_EQ(m_pTestee->getReader()->isExist(),1);
}

