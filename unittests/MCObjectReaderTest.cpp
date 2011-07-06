/*****************************************************************************
 *   Test Suite of The MCLinker Project,                                     *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#include <mcld/MC/MCObjectReader.h>
#include <mcld/Target/TargetObjectReader.h>
#include <MCObjectReaderTest.h>

using namespace mcld;
using namespace mcldtest;

TargetObjectReader *Test1Function(void){
	return new TargetObjectReader();
}

// Constructor can do set-up work for all test here.
MCObjectReaderTest::MCObjectReaderTest()
{
	// create testee. modify it if need
	m_pTestee = new MCObjectReader(Test1Function);
}

// Destructor can do clean-up work that doesn't throw exceptions here.
MCObjectReaderTest::~MCObjectReaderTest()
{
	delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void MCObjectReaderTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void MCObjectReaderTest::TearDown()
{
}

//==========================================================================//
// Testcases
//

TEST_F( MCObjectReaderTest, test ) {
 EXPECT_EQ(m_pTestee->getReader()->isExist(),1);
}

