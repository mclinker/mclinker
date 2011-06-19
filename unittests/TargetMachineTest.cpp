/*****************************************************************************
 *   Test Suite of The BOLD Project,                                         *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#include <TargetMachineTest.h>

using namespace mcld;
using namespace mcldTEST;


// Constructor can do set-up work for all test here.
TargetMachineTest::TargetMachineTest()
{
}

// Destructor can do clean-up work that doesn't throw exceptions here.
TargetMachineTest::~TargetMachineTest()
{
}

// SetUp() will be called immediately before each test.
void TargetMachineTest::SetUp()
{
}

// TearDown() will be called immediately after each test.
void TargetMachineTest::TearDown()
{
}

//==========================================================================//
// Testcases
//

TEST_F( TargetMachineTest, addPassesToEmitFile ) {
	mcld::addPassesToEmitFile();
}

